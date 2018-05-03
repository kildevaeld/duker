#include <curl/curl.h>
#include <dukext/utils.h>
#include <duktape.h>
#include <stdbool.h>

struct dukext_callback {
  duk_context *ctx;
  int ref; // Reference in the ref store to the callback
  void *data;
  duk_size_t size;
  int flag;
};

static duk_ret_t curl_client_dtor(duk_context *ctx) {

  if (duk_has_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("header"))) {
    duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("header"));
    struct curl_slist *list = (struct curl_slist *)duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    duk_del_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("header"));
    curl_slist_free_all(list);
  }

  duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("header"));

  if (duk_is_undefined(ctx, -1))
    return 0;

  CURL *curl = (CURL *)duk_get_pointer(ctx, -1);

  curl_easy_cleanup(curl);

  return 0;
}

static duk_ret_t curl_client_ctor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  duk_push_this(ctx);

  duk_push_c_function(ctx, curl_client_dtor, 1);
  duk_set_finalizer(ctx, -2);

  if (!duk_is_object_coercible(ctx, 0)) {
    duk_dup(ctx, 0);
  } else {
    duk_push_object(ctx);
  }

  duk_put_prop_string(ctx, -2, "options");

  CURL *curl = curl_easy_init();
  if (!curl) {
    duk_type_error(ctx, "could not initialize curl");
  }

  duk_push_pointer(ctx, curl);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("handle"));

  return 0;
}

static bool build_curl_request_header(duk_context *ctx, duk_idx_t idx,
                                      CURL *curl, char **err) {

  if (!duk_is_object(ctx, idx)) {
    *err = "header should be a object";
    return false;
  }

  duk_get_prop_string(ctx, idx, "header");

  duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);

  struct curl_slist *chunk = NULL;
  while (duk_next(ctx, -1, 1)) {
    duk_push_string(ctx, ": ");
    duk_dup(ctx, -3);
    duk_dup(ctx, -2);
    duk_join(ctx, 2);

    const char *header = duk_require_string(ctx, -1);
    chunk = curl_slist_append(chunk, header);
    duk_pop_3(ctx);
  }

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

  duk_pop_2(ctx);

  if (chunk) {
    duk_push_this(ctx);
    duk_push_pointer(ctx, chunk);
    duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("header"));
    duk_pop(ctx);
  }

  return true;
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow) {
  struct dukext_callback *progress = (struct dukext_callback *)p;

  duk_push_ref(progress->ctx, progress->ref);
  if (progress->flag == 0) {
    duk_push_number(progress->ctx, (double)dlnow);
    duk_push_number(progress->ctx, (double)dltotal);
  } else {
    duk_push_number(progress->ctx, (double)ulnow);
    duk_push_number(progress->ctx, (double)ultotal);
  }

  duk_pcall(progress->ctx, 2);

  duk_pop(progress->ctx);

  // printf("\rProcent %d/%d - %d/%d", dlnow, dltotal, ulnow, ultotal);
  return 0;
}

static int curl_progress_cb(void *clientp, double dltotal, double dlnow,
                            double ultotal, double ulnow) {
  return xferinfo(clientp, (curl_off_t)dltotal, (curl_off_t)dlnow,
                  (curl_off_t)ultotal, (curl_off_t)ulnow);
}

/*struct mdata {
  duk_context *ctx;
  char *data;
  int type; // 0 = buffer, 1 = callback
  int size;
  int ref;
};*/

static size_t curl_request_write_cb(char *ptr, size_t size, size_t nmemb,
                                    void *userdata) {
  size_t realsize = size * nmemb;
  struct dukext_callback *mem = (struct dukext_callback *)userdata;
  duk_push_ref(mem->ctx, mem->ref);

  if (duk_is_dynamic_buffer(mem->ctx, -1)) {
    mem->data = duk_resize_buffer(mem->ctx, -1, mem->size + realsize + 1);
    duk_pop(mem->ctx);
    if (mem->data == NULL) {

      printf("not enough memory (realloc returned NULL)\n");
      return 0;
    }

    memcpy(&(mem->data[mem->size]), ptr, realsize);
    mem->size += realsize;
    ((char *)mem->data)[mem->size] = 0;
  } else if (duk_is_function(mem->ctx, -1)) {
    char *buf = duk_push_fixed_buffer(mem->ctx, realsize);
    memcpy(buf, ptr, realsize);
    duk_ret_t ret = duk_pcall(mem->ctx, 1);
    if (ret != DUK_EXEC_SUCCESS) {
      duk_throw(mem->ctx);
    }
    duk_pop(mem->ctx);
  }
  /*if (mem->type == 1) {
    char *buf = duk_push_fixed_buffer(mem->ctx, realsize);
    memcpy(buf, ptr, realsize);
    duk_ret_t ret = duk_pcall(mem->ctx, 1);
    if (ret != DUK_EXEC_SUCCESS) {
      duk_throw(mem->ctx);
    }
    duk_pop(mem->ctx);
  } else {
    mem->data = duk_resize_buffer(mem->ctx, -1, mem->size + realsize + 1);
    duk_pop(mem->ctx);
    if (mem->data == NULL) {

      printf("not enough memory (realloc returned NULL)\n");
      return 0;
    }

    memcpy(&(mem->data[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
  }*/

  return realsize;
}

static size_t curl_request_header_cb(char *buffer, size_t size, size_t nitems,
                                     void *userdata) {
  size_t realsize = size * nitems;
  struct dukext_callback *mem = (struct dukext_callback *)userdata;

  if (realsize == 2)
    return realsize;

  char buf[realsize];
  memcpy(buf, buffer, realsize);

  duk_push_ref(mem->ctx, mem->ref);
  duk_push_lstring(mem->ctx, buf, realsize);

  duk_put_prop_index(mem->ctx, -2, duk_get_length(mem->ctx, -2));
  duk_pop(mem->ctx);

  return realsize;
}

static void build_curl_request_progress(duk_context *ctx, duk_idx_t idx,
                                        CURL *curl,
                                        struct dukext_callback *progress) {
  // Progress
  curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, curl_progress_cb);
  /* pass the struct pointer into the progress function */
  curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, progress);

#if LIBCURL_VERSION_NUM >= 0x072000
  /* xferinfo was introduced in 7.32.0, no earlier libcurl versions will
     compile as they won't have the symbols around.

     If built with a newer libcurl, but running with an older libcurl:
     curl_easy_setopt() will fail in run-time trying to set the new
     callback, making the older callback get used.

     New libcurls will prefer the new callback and instead use that one even
     if both callbacks are set. */

  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
  /* pass the struct pointer into the xferinfo function, note that this is
     an alias to CURLOPT_PROGRESSDATA */
  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, progress);
#endif

  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
}

static bool build_curl_request(duk_context *ctx, duk_idx_t idx, CURL *curl,
                               struct dukext_callback *body,
                               struct dukext_callback *header,
                               struct dukext_callback *progress, char **err) {

  duk_require_object(ctx, idx);
  duk_get_prop_string(ctx, idx, "url");
  const char *url = duk_require_string(ctx, -1);
  duk_pop(ctx);

  curl_easy_setopt(curl, CURLOPT_URL, url);

  if (duk_has_prop_string(ctx, idx, "method")) {
    duk_get_prop_string(ctx, idx, "method");
    duk_get_prop_string(ctx, -1, "toUpperCase");
    duk_call_method(ctx, idx);
    const char *method = duk_require_string(ctx, -1);
    duk_pop(ctx);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
  }

  if (duk_has_prop_string(ctx, idx, "header")) {
    if (!build_curl_request_header(ctx, idx, curl, err)) {
      return false;
    }
  }

  if (duk_has_prop_string(ctx, idx, "bodyWriter")) {
    duk_get_prop_string(ctx, idx, "bodyWriter");
    if (!duk_is_function(ctx, -1)) {
      *err = "data field is not a function";
      duk_pop(ctx);
      return false;
    }
    body->ref = duk_ref(ctx);
  } else {
    body->data = duk_push_dynamic_buffer(ctx, 0);
    body->size = 0;
    body->ref = duk_ref(ctx);
  }

  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_request_header_cb);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, header);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_request_write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, body);

  if (duk_has_prop_string(ctx, idx, "progress")) {

    duk_get_prop_string(ctx, idx, "progress");
    if (duk_is_function(ctx, -1)) {
      progress->ref = duk_ref(ctx);
      build_curl_request_progress(ctx, idx, curl, progress);
    } else {
      duk_pop(ctx);
    }
  }

  return true;
}

static void push_curl_response(duk_context *ctx, CURL *curl,
                               struct dukext_callback *body,
                               struct dukext_callback *header) {
  duk_push_object(ctx);

  duk_push_ref(ctx, body->ref);

  duk_put_prop_string(ctx, -2, "body");

  duk_push_ref(ctx, header->ref);
  duk_put_prop_string(ctx, -2, "header");

  long status;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  duk_push_int(ctx, status);
  duk_put_prop_string(ctx, -2, "statusCode");
}

/*
{
    url: "",
    header: {},
    bodyWriter: function(buffer)
}

*/
static duk_ret_t curl_client_request(duk_context *ctx) {

  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("handle"));
  duk_require_pointer(ctx, -1);

  CURL *curl = duk_get_pointer(ctx, -1);
  duk_pop_2(ctx);

  char *err;
  struct dukext_callback data[3]; // body, header, progress
  memset(&data, 0, sizeof(data));

  for (int i = 0; i < 3; i++)
    data[i].ctx = ctx;

  duk_push_array(ctx);
  data[1].ref = duk_ref(ctx);

  if (!build_curl_request(ctx, 0, curl, &data[0], &data[1], &data[2], &err)) {
    duk_type_error(ctx, "%s", err);
  }

  CURLcode ret = curl_easy_perform(curl);
  duk_push_this(ctx);
  if (duk_has_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("header"))) {
    duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("header"));
    struct curl_slist *list = (struct curl_slist *)duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    duk_del_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("header"));
    curl_slist_free_all(list);
  }
  duk_pop(ctx);

  if (ret != CURLE_OK) {
    duk_type_error(ctx, "somethin went wrong: %s", curl_easy_strerror(ret));
  }

  push_curl_response(ctx, curl, &data[0], &data[1]);

  for (int i = 0; i < 3; i++)
    duk_unref(ctx, data[i].ref);

  curl_easy_reset(curl);

  return 1;
}

void dukext_curl_push_client(duk_context *ctx) {

  duk_push_c_function(ctx, curl_client_ctor, 1);
  duk_push_object(ctx);

  duk_push_c_function(ctx, curl_client_request, 1);
  duk_put_prop_string(ctx, -2, "request");

  duk_put_prop_string(ctx, -2, "prototype");
}
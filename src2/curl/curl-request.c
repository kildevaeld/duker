#include <curl/curl.h>
#include <dukext/curl/utils.h>
#include <dukext/io/io.h>
#include <dukext/utils.h>
#include <duktape.h>
#include <stdbool.h>

struct curl_bag {
  dukext_bag_t *body;
  dukext_bag_t *header;
  dukext_bag_t *progress;
  dukext_bag_t *data;
};

static size_t curl_request_write_cb(char *ptr, size_t size, size_t nmemb,
                                    void *userdata) {
  size_t realsize = size * nmemb;
  dukext_bag_t *mem = (dukext_bag_t *)userdata;
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

  return realsize;
}

static size_t curl_request_header_cb(char *buffer, size_t size, size_t nitems,
                                     void *userdata) {
  size_t realsize = size * nitems;
  dukext_bag_t *mem = (dukext_bag_t *)userdata;

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

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow) {
  dukext_bag_t *progress = (dukext_bag_t *)p;

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

  return 0;
}

static int curl_progress_cb(void *clientp, double dltotal, double dlnow,
                            double ultotal, double ulnow) {
  return xferinfo(clientp, (curl_off_t)dltotal, (curl_off_t)dlnow,
                  (curl_off_t)ultotal, (curl_off_t)ulnow);
}

static size_t curl_read_cb(char *buffer, size_t size, size_t nitems, void *p) {
  dukext_bag_t *progress = (dukext_bag_t *)p;
  size_t buffer_size = size * nitems;

  duk_context *ctx = progress->ctx;

  duk_push_ref(progress->ctx, progress->ref);

  if (duk_is_string(progress->ctx, -1) || duk_is_buffer(ctx, -1)) {
    duk_size_t len = duk_get_length(ctx, -1);
    if (len == progress->size) {
      goto end;
    }

    duk_size_t re = len - progress->size;

    int read_l = re > buffer_size ? buffer_size : re;

    const char *data;
    duk_size_t size = len;
    if (duk_is_string(ctx, -1)) {
      data = duk_get_string(ctx, -1);
    } else {
      data = duk_get_buffer(ctx, -1, &size);
    }

    memcpy(buffer, data + progress->size, read_l);

    progress->size += read_l;
    duk_pop_2(ctx);

    return read_l;

  } else {
    duk_push_string(ctx, "read");
    duk_push_number(ctx, (duk_double_t)buffer_size);

    duk_call_prop(ctx, -3, 1);

    if (duk_is_null_or_undefined(ctx, -1)) {
      duk_pop(ctx);
      goto end;
    }

    duk_size_t size;
    const char *data;
    if (duk_is_string(ctx, -1)) {
      data = duk_get_string(ctx, -1);
      size = duk_get_length(ctx, -1);
    } else if (duk_is_buffer(ctx, -1)) {
      data = (const char *)duk_get_buffer(ctx, -1, &size);
    } else if (duk_is_buffer_data(ctx, -1)) {
      data = (const char *)duk_get_buffer_data(ctx, -1, &size);
    } else {
      return -1;
    }

    if (size == progress->size) {
      duk_pop(ctx);
      goto end;
    }

    duk_size_t re = size - progress->size;

    int read_l = re > buffer_size ? buffer_size : re;

    memcpy(buffer, data + progress->size, read_l);

    progress->size += read_l;
    duk_pop_2(ctx);
    return read_l;
  }

end:
  duk_pop(ctx);
  return 0;
}

static void build_curl_request_progress(CURL *curl, dukext_bag_t *progress) {
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

static bool duk_curl_request(duk_context *ctx, CURL *curl,
                             struct curl_bag *bags, char **err) {

  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  duk_idx_t idx = duk_normalize_index(ctx, -1);

  struct curl_slist *list = NULL;

  duk_get_prop_string(ctx, idx, "url");
  if (!duk_is_string(ctx, -1)) {
    *err = "url is not defined";
    duk_pop(ctx);
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_URL, duk_get_string(ctx, -1));
  duk_pop(ctx);

  duk_get_prop_string(ctx, idx, "method");
  if (!duk_is_string(ctx, -1)) {
    duk_pop(ctx);
    duk_push_string(ctx, "GET");
  }

  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, duk_get_string(ctx, -1));
  duk_pop(ctx);

  duk_get_prop_string(ctx, idx, "header");
  if (duk_is_undefined(ctx, -1)) {
    duk_pop(ctx);
  } else {
    duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
    while (duk_next(ctx, -1, 1)) {
      duk_push_string(ctx, ": ");
      duk_dup(ctx, -3);
      duk_dup(ctx, -2);
      duk_join(ctx, 2);

      list = curl_slist_append(list, duk_get_string(ctx, -1));

      duk_pop_3(ctx);
    }
  }

  // Setup body writer function
  dukext_bag_t *body = bags->body;
  body->data = duk_push_dynamic_buffer(ctx, 0);
  body->size = 0;
  body->ref = duk_ref(ctx);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_request_write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, body);

  // Setup header callbacks
  dukext_bag_t *header = bags->header;
  duk_push_array(ctx);

  header->size = 0;
  header->ref = duk_ref(ctx);

  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_request_header_cb);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, header);

  // Setup progress callback
  duk_get_prop_string(ctx, -1, "progress");
  if (duk_is_undefined(ctx, -1)) {
    duk_pop(ctx);
  } else {
    dukext_bag_t *progress = bags->progress;
    progress->ref = duk_ref(ctx);
    build_curl_request_progress(curl, progress);
  }

  // Setup read callback
  duk_get_prop_string(ctx, -1, "data");
  if (duk_is_undefined(ctx, -1)) {
    duk_pop(ctx);
  } else {
    if (duk_is_string(ctx, -1) || duk_is_buffer(ctx, -1)) {
      duk_size_t len = duk_get_length(ctx, -1);
      char num[64];
      sprintf(num, "Content-Length: %lu", len);
      list = curl_slist_append(list, num);
    } else {
      list = curl_slist_append(list, "Transfer-Encoding: chunked");
    }
    dukext_bag_t *data = bags->data;
    data->ref = duk_ref(ctx);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, curl_read_cb);
    curl_easy_setopt(curl, CURLOPT_READDATA, data);
  }

  // Set headers and save it on the
  // request for mem management

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
  duk_push_pointer(ctx, list);
  duk_put_prop_string(ctx, idx, DUK_HIDDEN_SYMBOL("requestheader"));

  duk_pop(ctx); // request

  return true;
}

static void push_curl_response(duk_context *ctx, CURL *curl, dukext_bag_t *body,
                               dukext_bag_t *header) {
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

static duk_ret_t curl_request(duk_context *ctx) {

  if (!duk_curl_is_request(ctx, 0)) {
    duk_type_error(ctx, "options not instanceof curl.Request");
  }
  int bag_size = 4;
  dukext_bag_t bags[bag_size];
  memset(bags, 0, sizeof(dukext_bag_t) * bag_size);
  for (int i = 0; i < bag_size; i++)
    bags[i].ctx = ctx;

  struct curl_bag state = {
      .body = &bags[0],
      .header = &bags[1],
      .progress = &bags[2],
      .data = &bags[3],
  };

  CURL *curl = curl_easy_init();
  char *err = NULL;
  duk_dup(ctx, 0);
  if (!duk_curl_request(ctx, curl, &state, &err)) {
    curl_easy_cleanup(curl);
    duk_type_error(ctx, "error %s", err);
  }

  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  CURLcode ret = curl_easy_perform(curl);

  if (ret != CURLE_OK) {
    for (int i = 0; i < bag_size; i++)
      duk_unref(ctx, bags[i].ref);
    curl_easy_cleanup(curl);
    duk_type_error(ctx, "something went wrong: %s", curl_easy_strerror(ret));
  }

  push_curl_response(ctx, curl, &bags[0], &bags[1]);

  for (int i = 0; i < bag_size; i++)
    duk_unref(ctx, bags[i].ref);

  curl_easy_cleanup(curl);

  return 1;
}

void dukext_curl_push_curl_request(duk_context *ctx) {
  duk_push_c_function(ctx, curl_request, 1);
}
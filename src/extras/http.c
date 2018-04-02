#include "http.h"
#include <curl/curl.h>

struct MemoryStruct {
  duk_context *ctx;
  duk_idx_t idx;
  unsigned char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = duk_resize_buffer(mem->ctx, mem->idx, mem->size + realsize + 1);
  // mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

static size_t header_callback(char *buffer, size_t size, size_t nitems,
                              void *userdata) {
  /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
  /* 'userdata' is set with CURLOPT_HEADERDATA */
  printf("%s\n", buffer);
  return nitems * size;
}

static duk_ret_t http_get(duk_context *ctx) {

  CURL *curl = curl_easy_init();

  const char *url = duk_require_string(ctx, 0);

  struct MemoryStruct chunk;

  chunk.ctx = ctx; /* will be grown as needed by the realloc above */
  chunk.size = 0;  /* no data at this point */
  chunk.memory = duk_push_dynamic_buffer(ctx, 0);
  chunk.idx = duk_normalize_index(ctx, -1);

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback);
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      curl_easy_cleanup(curl);
      duk_type_error(ctx, "error %s", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);

    return 1;
  }

  return 0;
}

const duk_function_list_entry http_fns[] = {{"get", http_get, 1}, {NULL}};

static duk_ret_t initialize_http_module(duk_context *ctx) {

  duk_idx_t idx = duk_push_object(ctx);

  duk_put_function_list(ctx, idx, http_fns);

  return 1;
}

int dukext_register_module_http(struct duker_s *ctx) {
  curl_global_init(CURL_GLOBAL_ALL);
  dukext_add_module_fn(ctx, "http", initialize_http_module);
  return 1;
}

int dukext_unregister_module_http(struct duker_s *ctx) {
  curl_global_cleanup();
  return 1;
}
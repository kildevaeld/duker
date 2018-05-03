#include <curl/curl.h>
#include <dukext/curl/curl.h>
#include <dukext/module.h>

extern void dukext_curl_push_client(duk_context *ctx);

static duk_ret_t dukext_curl_module(duk_context *ctx) {
  duk_push_object(ctx);

  dukext_curl_push_client(ctx);
  duk_put_prop_string(ctx, -2, "Client");

  return 1;
}

void dukext_curl_init(dukext_t *vm) {
  duk_context *ctx = dukext_get_ctx(vm);
  dukext_module_set(vm, "curl", dukext_curl_module);
}
#include <curl/curl.h>
#include <dukext/curl/curl.h>
#include <dukext/module.h>
#include <dukext/utils.h>

extern void dukext_curl_push_client(duk_context *ctx);
// extern void dukext_curl_push_form(duk_context *ctx);
// extern void dukext_curl_push_header(duk_context *ctx);
extern void dukext_curl_push_request(duk_context *ctx);
extern void dukext_curl_push_curl_request(duk_context *ctx);

static duk_ret_t dukext_curl_module(duk_context *ctx) {
  duk_push_object(ctx);

  dukext_curl_push_client(ctx);
  // dukext_curl_push_form(ctx);
  // dukext_curl_push_header(ctx);
  duk_put_prop_string(ctx, -2, "Client");

  dukext_curl_push_request(ctx);
  // We store on stash, for easy access
  duk_push_global_stash(ctx);
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, "CurlRequest");
  duk_pop(ctx);

  duk_put_prop_string(ctx, -2, "Request");

  dukext_curl_push_curl_request(ctx);
  duk_put_prop_string(ctx, -2, "req");

  return 1;
}

void dukext_curl_init(dukext_t *vm) {
  duk_context *ctx = dukext_get_ctx(vm);
  dukext_module_set(vm, "curl", dukext_curl_module);
}
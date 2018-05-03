#include <curl/curl.h>
#include <duktape.h>
static duk_ret_t curl_form_dtor(duk_context *ctx) {}

static duk_ret_t curl_form_ctor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  curl_mime *mime = curl_mime_init()

      return 0;
}

static duk_ret_t curl_form_add(duk_context *ctx) {}

void dukext_curl_push_form(duk_context *ctx) {
  duk_push_c_function(ctx, curl_form_ctor, 1);

  duk_push_object(ctx); // prototype

  duk_push_c_function(ctx, curl_form_add, 2);
  duk_put_prop_string(ctx, -2, "add");

  // duk_push_c_function(ctx, curl_header_get, 1);
  // duk_put_prop_string(ctx, -2, "get");

  // duk_push_c_function(ctx, curl_header_unset, 1);
  // duk_put_prop_string(ctx, -2, "unset");

  duk_put_prop_string(ctx, -2, "prototype");
}
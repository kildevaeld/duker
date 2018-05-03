#include <dukext/curl/utils.h>

duk_bool_t duk_curl_is_request(duk_context *ctx, duk_idx_t idx) {
  duk_dup(ctx, idx);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "CurlRequest");

  duk_bool_t ret = 1;
  if (!duk_instanceof(ctx, -3, -1)) {
    ret = 0;
  }
  duk_pop_3(ctx);
  return ret;
}
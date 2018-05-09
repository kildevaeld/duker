#include <duktape.h>

void dukext_initialize_context(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "dukext");
  duk_pop(ctx);
}
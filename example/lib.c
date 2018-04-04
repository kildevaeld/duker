#include <duker/duker.h>

int dukopen_plugin(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_string(ctx, "Hello, World");
  duk_put_prop_string(ctx, -2, "greeting");
  return 1;
}
#include <duker/duker.h>

int dukopen_plugin(duk_context *ctx) {
  duk_push_string(ctx, "Hello, World");

  return 1;
}
#include <duker/duker.h>

int init_module(duk_context *ctx) {
  duk_push_string(ctx, "Hello, World");

  return 1;
}
#include "uv-module.h"
#include "timers.h"
#include <duker/refs.h>

static duk_ret_t fs_module_init(duk_context *ctx) {

  duk_ret_t idx = duk_push_object(ctx);

  return 1;
}

int dk_register_module_uv(struct duker_s *ctx, uv_loop_t *loop) {

  dk_stash_set_ptr(ctx->ctx, "uv_loop", loop);

  init_timers(ctx, loop);
  // return add_module_fn(ctx, "fs", fs_module_init);

  return 0;
}

uv_loop_t *dk_module_uv_loop(struct duker_s *ctx) {
  return dk_stash_get_ptr(ctx->ctx, "uv_loop");
}
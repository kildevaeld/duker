#include "uv-module.h"
#include "fs.h"
#include "timers.h"
#include "types.h"
#include <duker/refs.h>

const char *kStashLoopKey = "uv_loop";
int dk_register_module_uv(struct duker_s *ctx, uv_loop_t *loop) {

  dk_stash_set_ptr(ctx->ctx, kStashLoopKey, loop);

  init_timers(ctx, loop);

  add_module_fn(ctx, "fs", fs_init_module);
  // return add_module_fn(ctx, "fs", fs_module_init);

  return 0;
}

uv_loop_t *dk_module_uv_loop(struct duker_s *ctx) {
  return dk_stash_get_ptr(ctx->ctx, kStashLoopKey);
}
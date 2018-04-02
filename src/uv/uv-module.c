#include "fs.h"
#include "promise.h"
#include "timers.h"
#include "types.h"
#include <duker/refs.h>
#include <duker/uv/uv-module.h>

const char *kStashLoopKey = "uv_loop";
int dukext_register_module_uv(duker_t *ctx, uv_loop_t *loop) {

  dukext_stash_set_ptr(ctx->ctx, kStashLoopKey, loop);

  init_timers(ctx, loop);

  // dukext_dump_context_stdout(ctx);
  duk_eval_lstring(ctx->ctx, promise_js, promise_js_len);
  duk_pop(ctx->ctx);

  add_module_fn(ctx, "fs", fs_init_module);
  // return add_module_fn(ctx, "fs", fs_module_init);

  return 0;
}

uv_loop_t *dukext_module_uv_loop(duker_t *ctx) {
  return dukext_stash_get_ptr(ctx->ctx, kStashLoopKey);
}
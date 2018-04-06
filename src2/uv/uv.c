#include "fs.h"
#include "timers.h"
#include <dukext/module.h>
#include <dukext/utils.h>
#include <dukext/uv/uv.h>

static duk_ret_t uv_fs_module(duk_context *ctx) {}

void dukext_uv_init(dukext_t *vm, uv_loop_t *loop) {

  duk_context *ctx = dukext_get_ctx(vm);

  if (duk_stash_get_ptr(ctx, "dukext_uv")) {
    return;
  }

  dukext_uv_loop_set(vm, loop);

  dukext_uv_timers_push(ctx);

  dukext_module_set(vm, "uv.fs", dukext_uv_fs);
}

void dukext_uv_loop_set(dukext_t *vm, uv_loop_t *loop) {
  duk_context *ctx = dukext_get_ctx(vm);
  duk_stash_set_ptr(ctx, "dukext_uv", loop);
}

uv_loop_t *dukext_uv_loop_get(dukext_t *vm) {
  duk_context *ctx = dukext_get_ctx(vm);
  return duk_stash_get_ptr(ctx, "dukext_uv");
}
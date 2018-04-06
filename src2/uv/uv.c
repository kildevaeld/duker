#include "fs.h"
#include "timers.h"
#include <csystem/path.h>
#include <dukext/module.h>
#include <dukext/utils.h>
#include <dukext/uv/uv.h>

static duk_ret_t duk_uv_cwd(duk_context *ctx) {
  duk_size_t size = PATH_MAX;
  char *buf = duk_push_dynamic_buffer(ctx, size);

  uv_cwd(buf, &size);

  duk_resize_buffer(ctx, -1, size);
  duk_buffer_to_string(ctx, -1);

  return 1;
}

static void dukext_uv_process_push(duk_context *ctx) {
  duk_push_global_object(ctx);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_uv_cwd, 0);
  duk_put_prop_string(ctx, -2, "cwd");
  duk_put_prop_string(ctx, -2, "process");
  duk_pop(ctx);
}

void dukext_uv_init(dukext_t *vm, uv_loop_t *loop) {

  duk_context *ctx = dukext_get_ctx(vm);

  if (duk_stash_get_ptr(ctx, "dukext_uv")) {
    return;
  }

  dukext_uv_loop_set(vm, loop);

  dukext_uv_timers_push(ctx);
  dukext_uv_process_push(ctx);

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
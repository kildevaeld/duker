#include <dukext/io/io.h>
#include <dukext/module.h>

extern void duk_io_push_writer(duk_context *ctx);
extern void duk_io_push_reader(duk_context *ctx);
extern void duk_io_push_file(duk_context *ctx);

duk_bool_t duk_io_is_writer(duk_context *ctx, duk_idx_t idx) {
  duk_dup(ctx, idx);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "Writer");

  duk_bool_t ret = 1;
  if (!duk_instanceof(ctx, -3, -1)) {
    ret = 0;
  }
  duk_pop_3(ctx);
  return ret;
}

duk_bool_t duk_io_is_reader(duk_context *ctx, duk_idx_t idx) {

  duk_dup(ctx, idx);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "Reader");

  duk_bool_t ret = 1;
  if (!duk_instanceof(ctx, -3, -1)) {
    ret = 0;
  }
  duk_pop_3(ctx);

  return ret;
}

static duk_ret_t dukext_io_module_init(duk_context *ctx) {

  duk_push_object(ctx);

  duk_io_push_writer(ctx);

  // We store on stash, for easy access
  duk_push_global_stash(ctx);
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, "Writer");
  duk_pop(ctx);

  duk_put_prop_string(ctx, -2, "Writer");

  duk_io_push_reader(ctx);

  // We store on stash, for easy access
  duk_push_global_stash(ctx);
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, "Reader");
  duk_pop(ctx);

  duk_put_prop_string(ctx, -2, "Reader");

  duk_io_push_file(ctx);
  duk_put_prop_string(ctx, -2, "File");

  return 1;
}

void dukext_io_init(dukext_t *vm) {
  dukext_module_set(vm, "io", dukext_io_module_init);
}
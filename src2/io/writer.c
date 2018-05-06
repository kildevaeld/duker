#include <dukext/io/stream.h>

static duk_ret_t duk_io_writer_ctor(duk_context *ctx) { return 0; }

static duk_ret_t duk_io_writer_write(duk_context *ctx) {
  duk_int_t magic = duk_get_current_magic(ctx);
  if (magic == 1) {
    duk_type_error(ctx, "cannot use Writer directly");
  }
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "_write");

  duk_dup(ctx, 0);
  duk_call_method(ctx, 1);
  duk_pop(ctx);

  return 1;
}

void duk_io_push_writer(duk_context *ctx) {
  duk_push_c_function(ctx, duk_io_writer_ctor, 0);

  duk_push_object(ctx);

  duk_push_c_function(ctx, duk_io_writer_write, 1);
  duk_put_prop_string(ctx, -2, "write");
  duk_push_c_function(ctx, duk_io_writer_write, 1);
  duk_set_magic(ctx, -1, 1);
  duk_put_prop_string(ctx, -2, "_write");

  duk_put_prop_string(ctx, -2, "prototype");
}
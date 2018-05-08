#include <dukext/io/io.h>
#include <stdio.h>

static duk_ret_t duk_io_file_dtor(duk_context *ctx) { return 0; }

static duk_ret_t duk_io_file_ctor(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);
  const char *mode = duk_require_string(ctx, 1);
  FILE *file = fopen(path, mode);

  if (!file) {
    duk_type_error(ctx, "could not open file");
  }
  duk_push_this(ctx);
  duk_push_pointer(ctx, file);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("FILE"));

  duk_push_c_function(ctx, duk_io_file_dtor, 1);
  duk_set_finalizer(ctx, -2);

  return 0;
}

static duk_ret_t duk_io_file_read(duk_context *ctx) { return 0; }

static duk_ret_t duk_io_file_write(duk_context *ctx) { return 0; }

static duk_ret_t duk_io_file_seek(duk_context *ctx) { return 0; }

static duk_ret_t duk_io_file_tell(duk_context *ctx) { return 0; }

void duk_io_push_file(duk_context *ctx) {
  duk_push_c_function(ctx, duk_io_file_ctor, 0);

  duk_push_object(ctx);

  duk_push_c_function(ctx, duk_io_file_read, 2);
  duk_put_prop_string(ctx, -2, "read");

  duk_push_c_function(ctx, duk_io_file_write, 2);
  duk_put_prop_string(ctx, -2, "write");

  duk_push_c_function(ctx, duk_io_file_seek, 2);
  duk_put_prop_string(ctx, -2, "seek");

  duk_push_c_function(ctx, duk_io_file_tell, 0);
  duk_put_prop_string(ctx, -2, "tell");

  duk_put_prop_string(ctx, -2, "prototype");
}
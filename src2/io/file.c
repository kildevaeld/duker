#include <dukext/io/io.h>
#include <dukext/utils.h>
#include <stdio.h>

#define PUSH_FILE                                                              \
  duk_push_this(ctx);                                                          \
  if (!duk_has_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("FILE"))) {              \
    duk_type_error(ctx, "file is closed");                                     \
  }                                                                            \
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("FILE"));                     \
  FILE *file = (FILE *)duk_get_pointer(ctx, -1);                               \
  duk_pop_2(ctx);

static duk_ret_t duk_io_file_dtor(duk_context *ctx) {
  if (duk_has_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("FILE"))) {
    duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("FILE"));
    FILE *file = (FILE *)duk_get_pointer(ctx, -1);
    fclose(file);
    duk_del_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("FILE"));
  }
  return 0;
}

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

static duk_ret_t duk_io_file_read(duk_context *ctx) {
  int len = duk_require_int(ctx, 0);
  int offs = 0;
  if (duk_is_number(ctx, 1)) {
    offs = len;
    len = duk_require_int(ctx, 1);
  }

  PUSH_FILE

  fseek(file, offs, SEEK_SET);
  void *ptr = duk_push_fixed_buffer(ctx, len);
  fread(ptr, len, 1, file);

  return 1;
}

static duk_ret_t duk_io_file_write(duk_context *ctx) {
  void *data;
  duk_size_t size;

  if (duk_is_buffer(ctx, 0)) {
    data = duk_get_buffer(ctx, 0, &size);
  } else if (duk_is_buffer_data(ctx, 0)) {
    data = duk_get_buffer_data(ctx, -1, &size);
  } else if (duk_is_string(ctx, 0)) {
    data = (void *)duk_get_string(ctx, 0);
    size = duk_get_length(ctx, 0);
  }

  PUSH_FILE

  fwrite(data, size, 1, file);
  if (ferror(file) != 0) {
    duk_type_error(ctx, "could not write to file");
  }

  duk_push_this(ctx);

  return 1;
}

static duk_ret_t duk_io_file_seek(duk_context *ctx) {
  int to = duk_require_int(ctx, 0);
  int whence = duk_require_int(ctx, 1);

  PUSH_FILE

  if (fseek(file, to, whence)) {
    duk_type_error(ctx, "could not seek file");
  }

  duk_push_this(ctx);

  return 1;
}

static duk_ret_t duk_io_file_tell(duk_context *ctx) {

  PUSH_FILE

  long pos = ftell(file);
  duk_push_number(ctx, (double)pos);

  return 1;
}

static duk_ret_t duk_io_file_rewind(duk_context *ctx) {

  PUSH_FILE
  rewind(file);

  return 1;
}

static duk_ret_t duk_io_file_flush(duk_context *ctx) {

  PUSH_FILE
  if (fflush(file)) {
    duk_type_error(ctx, "could not flush file");
  }

  return 1;
}

static duk_ret_t duk_io_file_close(duk_context *ctx) {

  PUSH_FILE
  if (fclose(file) == EOF) {
    duk_type_error(ctx, "could not close file");
  }
  duk_push_this(ctx);
  duk_del_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("FILE"));

  return 0;
}

void duk_io_push_file(duk_context *ctx) {
  duk_push_c_function(ctx, duk_io_file_ctor, 2);

  duk_push_object(ctx);

  duk_push_c_function(ctx, duk_io_file_read, 2);
  duk_put_prop_string(ctx, -2, "read");

  duk_push_c_function(ctx, duk_io_file_write, 2);
  duk_put_prop_string(ctx, -2, "write");

  duk_push_c_function(ctx, duk_io_file_seek, 2);
  duk_put_prop_string(ctx, -2, "seek");

  duk_push_c_function(ctx, duk_io_file_tell, 0);
  duk_put_prop_string(ctx, -2, "tell");

  duk_push_c_function(ctx, duk_io_file_close, 0);
  duk_put_prop_string(ctx, -2, "close");

  duk_push_c_function(ctx, duk_io_file_flush, 0);
  duk_put_prop_string(ctx, -2, "flush");

  duk_push_c_function(ctx, duk_io_file_rewind, 0);
  duk_put_prop_string(ctx, -2, "rewind");

  duk_put_prop_string(ctx, -2, "prototype");
}
#include <csystem/file.h>
#include <csystem/path.h>
#include <duktape.h>

static duk_ret_t defs_file_write(duk_context *ctx) {
  // [ path encoding]
  bool c = false;
  const char *path = duk_require_string(ctx, 0);

  if (!cs_path_is_abs(path)) {
    path = cs_path_resolve(path, NULL);
    if (c)
      c = true;
    if (!path)
      duk_type_error(ctx, "could not resolve file");
  }

  const char *data;
  duk_size_t size;
  if (duk_is_string(ctx, 1)) {
    data = duk_get_string(ctx, 1);
    size = strlen(data);
  } else if (duk_is_buffer(ctx, 1)) {
    data = duk_get_buffer(ctx, 1, &size);
  }

  cs_write_file(path, data, size);

  if (c)
    free((char *)path);

  return 0;
}

static duk_ret_t defs_file_read(duk_context *ctx) {
  // [ path encoding]
  bool c = false;
  const char *path = duk_require_string(ctx, 0);
  const char *enc = duk_get_string(ctx, 1);

  if (!cs_path_is_abs(path)) {
    path = cs_path_abs(path, NULL, 0);
    if (c)
      c = true;
    if (!path)
      duk_type_error(ctx, "could not resolve file");
  }

  if (!cs_file_exists(path)) {
    if (c)
      free((char *)path);
    duk_type_error(ctx, "ENOTFOUND");
  }
  int size = cs_file_size(path);
  char *buf = duk_push_fixed_buffer(ctx, size);

  if (!cs_read_file(path, buf, size, &size)) {
    if (c)
      free((char *)path);
    duk_type_error(ctx, "could not read file");
  }

  if (c)
    free((char *)path);

  duk_push_buffer_object(ctx, -1, 0, size, DUK_BUFOBJ_NODEJS_BUFFER);

  return 1;
}

duk_ret_t dukext_module_fs(duk_context *ctx) {
  duk_push_object(ctx);

  duk_push_c_function(ctx, defs_file_write, 2);
  duk_put_prop_string(ctx, -2, "writeFile");

  duk_push_c_function(ctx, defs_file_read, 2);
  duk_put_prop_string(ctx, -2, "readFile");

  return 1;
}
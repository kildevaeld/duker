#include "fs.h"
#include "../module.h"
#include <csystem/file.h>
#include <errno.h>

static duk_ret_t fs_read_file(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);
  int size = cs_file_size(path);
  if (size == -1) {
    duk_type_error(ctx, "file does not exists");
  }

  unsigned char *ptr = (unsigned char *)duk_push_fixed_buffer(ctx, size);
  if (!cs_read_file(path, (char *)ptr, size, NULL)) {
    duk_pop(ctx);
    return 0;
  }
  duk_push_buffer_object(ctx, -1, 0, size, DUK_BUFOBJ_NODEJS_BUFFER);
  return 1;
}

static duk_ret_t fs_write_file(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);

  const void *data = NULL;
  duk_size_t data_size = 0;

  if (duk_is_string(ctx, 1)) {
    data = duk_require_string(ctx, 1);
    data_size = strlen(data);
  } else if (duk_is_buffer_data(ctx, 1)) {
    data = duk_require_buffer_data(ctx, 1, &data_size);
  }

  if (!cs_write_file(path, (const char *)data, data_size)) {
    duk_type_error(ctx, "could not write file: %s", strerror(errno));
  }

  return 0;
}

#include <dirent.h>

static duk_ret_t fs_mkdir(duk_context *ctx) {
  const char *path = duk_require_string(ctx, 0);
  const int mode = duk_require_int(ctx, 1);

  if (!mkdir(path, mode)) {
    duk_type_error(ctx, "could not create directory: %s", strerror(errno));
  }

  return 0;
}

static duk_ret_t fs_read_dir(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);

  DIR *dir;
  struct dirent *dp;
  char *file_name;
  dir = opendir(path);

  duk_idx_t idx = duk_push_array(ctx);

  int i = 0;
  while ((dp = readdir(dir)) != NULL) {
    if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
    } else {
      duk_push_string(ctx, dp->d_name);
      duk_put_prop_index(ctx, idx, i++);
    }
  }
  closedir(dir);

  return 1;
}

static duk_ret_t fs_module_init(duk_context *ctx) {

  duk_ret_t idx = duk_push_object(ctx);

  duk_push_c_function(ctx, fs_read_file, 2);
  duk_put_prop_string(ctx, idx, "readFileSync");

  duk_push_c_function(ctx, fs_write_file, 2);
  duk_put_prop_string(ctx, idx, "writeFileSync");

  duk_push_c_function(ctx, fs_read_dir, 1);
  duk_put_prop_string(ctx, idx, "readdirSync");

  duk_push_c_function(ctx, fs_mkdir, 2);
  duk_put_prop_string(ctx, idx, "mkdirSync");

  return 1;
}

int dk_register_module_fs(struct duker_s *ctx) {
  return add_module_fn(ctx, "fs", fs_module_init);
}
#include "fs.h"
#include <csystem/file.h>

static char *_read_file(const char *filename, char *buffer) {

  int string_size, read_size;
  FILE *handler = fopen(filename, "r");

  if (handler) {
    read_size = fread(buffer, sizeof(char), string_size, handler);
    fclose(handler);
  } else {
    return NULL;
  }

  return buffer;
}

static duk_ret_t fs_read_file(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);
  int size = cs_file_size(path);
  if (size == -1) {
    duk_type_error(ctx, "file does not exists");
  }

  unsigned char *ptr = (unsigned char *)duk_push_fixed_buffer(ctx, size);
  if (!_read_file(path, ptr)) {
    duk_pop(ctx);
    return 0;
  }
  duk_push_buffer_object(ctx, -1, 0, size, DUK_BUFOBJ_NODEJS_BUFFER);
  return 1;
}
#include <dirent.h>

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

  duk_push_c_function(ctx, fs_read_dir, 1);
  duk_put_prop_string(ctx, idx, "readdirSync");

  return 1;
}

int dk_register_module_fs(struct duker_s *ctx) {
  return add_module_fn(ctx, "fs", fs_module_init);
}
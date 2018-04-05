#include "commonjs_file.h"
#include <csystem/features.h>
#include <dukext/module.h>
#include <dukext/utils.h>

#ifdef CS_PLATFORM_DARWIN
#define CS_LIBRARY_EXT ".dylib"
#elif CS_PLATFORM_POSIX
#define CS_LIBRARY_EXT ".so"
#endif

static bool file_exists(char *buffer, size_t len, const char *ext) {
  size_t elen = strlen(ext);
  strcpy(buffer + len, ext);
  buffer[len + elen] = '\0';
  if (!cs_file_exists(buffer)) {
    char buf[len + elen + 4 + 1];

    int bidx, didx;

    int blen = cs_path_base(buffer, &bidx);
    if (blen == 0) {
      return false;
    }

    didx = cs_path_dir(buffer);
    memcpy(buf, buffer, didx);
    memcpy(buf + didx, "/lib", 4);
    memcpy(buf + didx + 4, buffer + bidx, blen);
    buf[len + elen + 3] = '\0';
    if (!cs_file_exists(buf)) {
      return false;
    }
    strcpy(buffer, buf);
  }
  return true;
}

duk_ret_t cjs_resolve_file(duk_context *ctx) {

  duk_get_prop_string(ctx, 0, "protocol");
  duk_get_prop_index(ctx, -1, 2);
  const char *full_file = duk_require_string(ctx, -1);
  duk_pop_2(ctx);

  duk_idx_t array = duk_push_array(ctx);

  int ext_idx = -1;
  int len = cs_path_ext(full_file, &ext_idx);

  int al = 0;
  if (len == 0) {
    len = strlen(full_file);
    char buf[len + 15];
    strcpy(buf, full_file);
    if (file_exists(buf, len, CS_LIBRARY_EXT)) {
      duk_push_string(ctx, buf);
      duk_put_prop_index(ctx, array, al++);
      strcpy(buf, full_file);
    }
    if (file_exists(buf, len, ".js")) {
      duk_push_string(ctx, buf);
      duk_put_prop_index(ctx, array, al++);
    }

  } else if (cs_file_exists(full_file)) {
    duk_push_string(ctx, full_file);
    duk_put_prop_index(ctx, array, al++);
  }

  duk_put_prop_string(ctx, 0, "files");

  return 0;
}

duk_ret_t cjs_load_file(duk_context *ctx) {

  // [ info module ]

  return 1;
}

duk_ret_t cjs_resolve_module(duk_context *ctx) {
  duk_get_prop_string(ctx, 0, "protocol");
  duk_get_prop_index(ctx, -1, 2);

  const char *name = duk_require_string(ctx, -1);
  // duk_pop(ctx);

  if (!dukext_module_has(duk_get_dukext(ctx), name)) {
    duk_type_error(ctx, "could not find module: '%s'", name);
  }

  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "find_module");
  duk_dup(ctx, -3);
  duk_ret_t ret = duk_pcall(ctx, 1);

  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }

  duk_put_prop_string(ctx, 0, "module");

  return 0;
}
duk_ret_t cjs_load_module(duk_context *ctx) { return 1; }
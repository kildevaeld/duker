#include "path.h"
#include "../module.h"
#include <csystem/path.h>
#include <duker/schema.h>

static duk_ret_t path_basename(duk_context *ctx) {

  dschema_check(ctx,
                (const dukext_schema_entry[]){{"path", duk_is_string}, {NULL}});

  const char *path = duk_require_string(ctx, 0);

  int len = 0, idx = 0;
  if (!(len = cs_path_base(path, &idx))) {
    return 0;
  }

  duk_push_lstring(ctx, path + idx, len);

  return 1;
}

static duk_ret_t path_dirname(duk_context *ctx) {
  dschema_check(ctx,
                (const dukext_schema_entry[]){{"path", duk_is_string}, {NULL}});

  const char *path = duk_require_string(ctx, 0);

  int len = 0;
  if (!(len = cs_path_dir(path))) {
    return 0;
  }

  duk_push_lstring(ctx, path, len);

  return 1;
}

static duk_ret_t path_join(duk_context *ctx) {
  int len = duk_get_top(ctx);
  const char *paths[len + 1];
  int l = 0;
  for (int i = 0; i < len; i++) {
    paths[i] = duk_require_string(ctx, i);
    l += strlen(paths[i]) + 2;
  }
  paths[len] = NULL;

  // char buffer[l + 1];
  char *buffer = cs_path_join_array(NULL, paths);
  if (!buffer)
    return 0;

  duk_push_string(ctx, buffer);

  return 1;
}

static duk_ret_t path_module_init(duk_context *ctx) {

  duk_idx_t idx = duk_push_object(ctx);

  duk_push_c_function(ctx, path_basename, 1);
  duk_put_prop_string(ctx, idx, "basename");

  duk_push_c_function(ctx, path_dirname, 1);
  duk_put_prop_string(ctx, idx, "dirname");

  duk_push_c_function(ctx, path_join, DUK_VARARGS);
  duk_put_prop_string(ctx, idx, "join");

  return 1;
}

int dk_register_module_path(struct duker_s *ctx) {
  return add_module_fn(ctx, "path", path_module_init);
}
#include "commonjs_module.h"
#include <dukext/utils.h>

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

duk_ret_t cjs_load_module(duk_context *ctx) {

  duk_get_prop_string(ctx, 0, "module");
  duk_require_function(ctx, -1);

  if (duk_is_c_function(ctx, -1)) {
    duk_dup(ctx, 1);
    duk_call(ctx, 1);
    duk_put_prop_string(ctx, 1, "exports");
  } else {
    (void)duk_get_prop_string(ctx, 1, "exports");  /* exports */
    (void)duk_get_prop_string(ctx, 1, "require");  /* require */
    duk_dup(ctx, 1);                               /* module */
    (void)duk_get_prop_string(ctx, 1, "filename"); /* __filename */
    duk_push_undefined(ctx);                       /* __dirname */
    duk_call(ctx, 5);
  }

  return 0;
}
#include "definitions.h"
#include <dukext/module.h>
#include <dukext/utils.h>

bool dukext_set_module_resolver(dukext_t *vm, const char *protocol,
                                dukext_module_resolve_cb resolve,
                                dukext_module_load_cb load) {
  duk_context *ctx = vm->ctx;
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "resolvers");

  bool ret = true;

  if (duk_has_prop_string(ctx, -1, protocol)) {
    ret = false;
    goto end;
  }

  duk_push_object(ctx);
  duk_push_c_lightfunc(ctx, resolve, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_lightfunc(ctx, resolve, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "load");
  duk_push_string(ctx, protocol);
  duk_put_prop_string(ctx, -2, "protocol");
  duk_put_prop_string(ctx, -2, protocol);

end:
  duk_pop_2(ctx);
  return ret;
}

bool dukext_module_set(dukext_t *vm, const char *name,
                       dukext_module_initializer init) {

  duk_context *ctx = vm->ctx;

  duk_push_global_stash(ctx);

  if (!duk_get_prop_string(ctx, -1, "modules")) {
    duk_pop(ctx);
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "modules");
  }

  if (duk_has_prop_string(ctx, -1, name)) {
    duk_pop_2(ctx);
    return false;
  }

  duk_push_c_lightfunc(ctx, init, 0, 0, 0);
  duk_put_prop_string(ctx, -2, name);

  duk_pop_2(ctx);
}

duk_ret_t dukextp_module_push(duk_context *ctx) {

  const char *name = duk_require_string(ctx, 0);

  duk_push_global_stash(ctx);

  if (!duk_get_prop_string(ctx, -1, "modules")) {
    duk_pop(ctx);
    duk_type_error(ctx, "could not load module %s", name);
  }

  if (!duk_has_prop_string(ctx, -1, name)) {
    duk_type_error(ctx, "could not load module %s", name);
  }

  duk_get_prop_string(ctx, -1, name);

  return 1;
}

bool dukext_module_has(dukext_t *vm, const char *name) {
  duk_context *ctx = vm->ctx;

  duk_push_global_stash(ctx);

  bool ret = false;

  if (!duk_get_prop_string(ctx, -1, "modules")) {
    goto end;
  }

  if (duk_has_prop_string(ctx, -1, name)) {
    ret = true;
  }

end:
  duk_pop_2(ctx);
  return ret;
}
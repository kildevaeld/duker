#include <dukext/types.h>
#include <dukext/utils.h>

#define DUKEXT_GUARD                                                           \
  duk_push_global_stash(ctx);                                                  \
  if (!duk_has_prop_string(ctx, -1, "dukext")) {                               \
    return false;                                                              \
  }                                                                            \
  duk_get_prop_string(ctx, -1, "dukext");                                      \
  duk_remove(ctx, -2);

static bool dukext_push_entry(duk_context *ctx, const char *name) {
  DUKEXT_GUARD

  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);
}

static bool dukext_get_entry(duk_context *ctx, const char *name) {
  DUKEXT_GUARD
  bool ret = true;

  if (!duk_has_prop_string(ctx, -1, name)) {
    duk_pop(ctx);
    return false;
  }

  duk_get_prop_string(ctx, -1, name);

  duk_remove(ctx, -2);

  return true;
}

static bool dukext_has_entry(duk_context *ctx, const char *name) {
  DUKEXT_GUARD

  duk_bool_t b = duk_has_prop_string(ctx, -1, name);
  duk_pop(ctx);
  return b;
}

bool dukext_initialize_types(duk_context *ctx) {

  if (dukext_has_entry(ctx, "Types")) {
    return false;
  }
  duk_push_object(ctx);
  dukext_push_entry(ctx, "Types");

  return true;
}

bool duk_global_type_register(duk_context *ctx, const char *name) {
  if (!dukext_get_entry(ctx, "Types")) {
    return false;
  }
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);
  return true;
}
bool duk_global_type_unregister(duk_context *ctx, const char *name) {
  if (!dukext_get_entry(ctx, "Types")) {
    return false;
  }
  duk_del_prop_string(ctx, -1, name);
  return true;
}

bool duk_global_type_get(duk_context *ctx, const char *name) {
  if (!dukext_get_entry(ctx, "Types")) {
    return false;
  }
  if (!duk_has_prop_string(ctx, -1, name)) {
    return false;
  }
  duk_get_prop_string(ctx, -1, name);
  duk_remove(ctx, -2);
  return true;
}
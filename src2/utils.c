#include <dukext/utils.h>

void duk_stash_set_ptr(duk_context *ctx, const char *name, void *ptr) {
  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop(ctx);
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "globals");
  }

  duk_push_pointer(ctx, ptr);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);
}
void *duk_stash_get_ptr(duk_context *ctx, const char *name) {

  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop_2(ctx);
    return NULL;
  }

  duk_get_prop_string(ctx, -1, name);
  if (duk_is_null_or_undefined(ctx, -1)) {
    return NULL;
  }
  void *c = duk_to_pointer(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

void duk_stash_rm_ptr(duk_context *ctx, const char *name) {
  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop_2(ctx);
    return;
  }

  duk_del_prop_string(ctx, -1, name);
  duk_pop_2(ctx);
}

const char *duk_get_main(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1,
                      "\xff"
                      "mainModule");

  duk_get_prop_string(ctx, -1, "filename");
  const char *c = duk_get_string(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

dukext_t *duk_get_dukext(duk_context *ctx) {
  return duk_stash_get_ptr(ctx, "dukext_vm");
}

void dukext_dump_context_stdout(duk_context *ctx) {
  duk_push_context_dump(ctx);
  printf("%s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);
}
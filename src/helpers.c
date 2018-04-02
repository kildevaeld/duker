#include "helpers.h"

const char *get_main(duk_context *ctx) {

  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1,
                      "\xff"
                      "mainModule");

  duk_get_prop_string(ctx, -1, "filename");
  const char *c = duk_get_string(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

duker_t *get_duker(duk_context *ctx) {
  /*duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "duker");
  duker_t *c = (duker_t *)duk_to_pointer(ctx, -1);
  duk_pop_2(ctx);
  return c;*/
  return dukext_stash_get_ptr(ctx, "duker");
}
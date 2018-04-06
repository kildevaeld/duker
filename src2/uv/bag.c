#include "bag.h"
#include <dukext/utils.h>

uv_bag_t *dukext_uv_bag_get(duk_context *ctx, int ref) {
  uv_bag_t *bag = duk_push_fixed_buffer(ctx, sizeof(uv_bag_t));
  bag->_selfref = duk_ref(ctx);
  bag->ctx = ctx;
  bag->ref = ref;
  duk_pop(ctx);
  return bag;
}

void dukext_uv_bag_put(uv_bag_t *bag) {
  if (!bag)
    return;

  duk_context *ctx = bag->ctx;
  bag->ctx = NULL;
  duk_unref(ctx, bag->ref);
  duk_unref(ctx, bag->_selfref);
}
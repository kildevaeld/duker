#pragma once
#include <duktape.h>

typedef struct {
  int ref;
  duk_context *ctx;
  int _selfref;
} uv_bag_t;

uv_bag_t *dukext_uv_bag_get(duk_context *ctx, int ref);

void dukext_uv_bag_put(uv_bag_t *bag);
#pragma once
#include <duktape.h>

struct bag_ {
  duk_context *ctx;
  int ref;
  int repeat;
};
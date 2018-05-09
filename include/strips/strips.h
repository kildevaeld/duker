#pragma once
#include <duktape.h>

typedef enum strips_ret_t { STRIPS_OK } strips_ret_t;

strips_ret_t strips_initialize(duk_context *ctx);

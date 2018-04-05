#pragma once
#include <dukext/module.h>

duk_ret_t cjs_resolve_file(duk_context *ctx);
duk_ret_t cjs_load_file(duk_context *ctx);

duk_ret_t cjs_resolve_module(duk_context *ctx);
duk_ret_t cjs_load_module(duk_context *ctx);
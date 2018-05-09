#pragma once
#include <duktape.h>
#include <stdbool.h>

bool duk_global_type_register(duk_context *ctx, const char *name);
bool duk_global_type_unregister(duk_context *Ctx, const char *name);
bool duk_global_type_get(duk_context *ctx, const char *name);
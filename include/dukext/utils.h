#pragma once
#include <dukext/dukext.h>
#include <duktape.h>

// Global stash
void duk_stash_set_ptr(duk_context *ctx, const char *name, void *ptr);
void *duk_stash_get_ptr(duk_context *ctx, const char *name);
void duk_stash_rm_ptr(duk_context *ctx, const char *name);

const char *duk_get_main(duk_context *ctx);
dukext_t *duk_get_dukext(duk_context *ctx);

void dukext_dump_context_stdout(duk_context *ctx);
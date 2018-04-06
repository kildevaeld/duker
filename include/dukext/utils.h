#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <dukext/dukext.h>
#include <duktape.h>

// Global stash
void duk_stash_set_ptr(duk_context *ctx, const char *name, void *ptr);
void *duk_stash_get_ptr(duk_context *ctx, const char *name);
void duk_stash_rm_ptr(duk_context *ctx, const char *name);

// Create a global array refs in the heap stash.
void duk_ref_setup(duk_context *ctx);
// like luaL_ref, but assumes storage in "refs" property of heap stash
int duk_ref(duk_context *ctx);
void duk_push_ref(duk_context *ctx, int ref);
void duk_unref(duk_context *ctx, int ref);

const char *duk_get_main(duk_context *ctx);
dukext_t *duk_get_dukext(duk_context *ctx);

void dukext_dump_context_stdout(duk_context *ctx);

#ifdef __cplusplus
}
#endif
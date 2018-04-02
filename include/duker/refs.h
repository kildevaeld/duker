#pragma once
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

// Create a global array refs in the heap stash.
void dukext_ref_setup(duk_context *ctx);
// like luaL_ref, but assumes storage in "refs" property of heap stash
int dukext_ref(duk_context *ctx);
void dukext_push_ref(duk_context *ctx, int ref);
void dukext_unref(duk_context *ctx, int ref);

#ifdef __cplusplus
}
#endif
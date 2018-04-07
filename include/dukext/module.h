#pragma once
#include <dukext/dukext.h>
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef duk_ret_t (*dukext_module_resolve_cb)(duk_context *);
typedef duk_ret_t (*dukext_module_load_cb)(duk_context *);

bool dukext_set_module_resolver(dukext_t *vm, const char *protocol,
                                dukext_module_resolve_cb resolve,
                                dukext_module_load_cb load);

bool dukext_module_set(dukext_t *vm, const char *name,
                       dukext_module_initializer init);

bool dukext_module_has(dukext_t *vm, const char *name);

duk_ret_t dukextp_module_push(duk_context *ctx);

#ifdef __cplusplus
}
#endif
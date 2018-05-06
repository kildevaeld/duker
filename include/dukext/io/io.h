#pragma once
#include <dukext/dukext.h>
#ifdef __cplusplus
extern "C" {
#endif

duk_bool_t duk_io_is_writer(duk_context *ctx, duk_idx_t idx);
duk_bool_t duk_io_is_reader(duk_context *ctx, duk_idx_t idx);

void dukext_io_init(dukext_t *vm);

#ifdef __cplusplus
}
#endif
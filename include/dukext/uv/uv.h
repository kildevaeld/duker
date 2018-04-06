#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <dukext/dukext.h>
#include <uv.h>

void dukext_uv_init(dukext_t *vm, uv_loop_t *loop);
void dukext_uv_loop_set(dukext_t *vm, uv_loop_t *loop);
uv_loop_t *dukext_uv_loop_get(dukext_t *vm);

#ifdef __cplusplus
}
#endif
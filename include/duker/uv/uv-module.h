#pragma once
#include <duker/duker.h>
#include <uv.h>

int dk_register_module_uv(duker_t *ctx, uv_loop_t *loop);

uv_loop_t *dk_module_uv_loop(duker_t *ctx);
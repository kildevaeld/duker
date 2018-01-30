#pragma once
#include "../type.h"
#include <uv.h>


int dk_register_module_uv(struct duker_s *ctx, uv_loop_t *loop);

uv_loop_t *dk_module_uv_loop(struct duker_s *ctx);
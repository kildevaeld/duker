#pragma once
#include "definitions.h"

void dukextp_init_commonjs(dukext_t *vm);
duk_ret_t dukextp_commonjs_eval_main(duk_context *ctx, const char *path);
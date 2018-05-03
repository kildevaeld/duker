#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <dukext/dukext.h>

duk_bool_t duk_curl_is_request(duk_context *ctx, duk_idx_t idx);

#ifdef __cplusplus
}
#endif
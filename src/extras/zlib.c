#include "zlib.h"
#include "gzip.h"
#include <miniz.h>
static const char *duk_str_or_buffer(duk_context *ctx, duk_idx_t idx,
                                     size_t *len) {
  if (duk_is_string(ctx, idx)) {
    const char *out = duk_require_string(ctx, idx);
    *len = strlen(out);
    return out;
  } else if (duk_is_buffer(ctx, idx)) {
    return duk_require_buffer(ctx, idx, (duk_size_t *)len);
  }
  return NULL;
}

static duk_ret_t zlib_create_deflate(duk_context *ctx) { return 0; }

static duk_ret_t zlib_deflate(duk_context *ctx) {

  const char *input = NULL;
  size_t len = 0;

  if (!(input = duk_str_or_buffer(ctx, 0, &len))) {
    duk_type_error(ctx, "invalid argument");
  }
  size_t clen = compressBound(len);
  unsigned char *buffer = duk_push_dynamic_buffer(ctx, clen + 18);
  cs_gzip_write_hdr(buffer);

  int status =
      compress2(buffer + 10, (mz_ulong *)&clen, (const unsigned char *)input,
                len, Z_DEFAULT_COMPRESSION);

  uint32_t crc = mz_crc32(0L, input, len);

  // crc = mz_crc32(crc, input, len);

  cs_gzip_ctx_t gzip;
  gzip.len = len;
  gzip.crc = crc;

  cs_gzip_write_trail(&gzip, buffer + 10 + clen);

  duk_resize_buffer(ctx, -1, clen + 18);

  if (status != Z_OK) {
    duk_type_error(ctx, "could not compress");
  }

  return 1;
}

const duk_function_list_entry zlib_fns[] = {
    {"createDeflate", zlib_create_deflate, 1},
    {"deflate", zlib_deflate, 1},
    {NULL}};

static duk_ret_t initialize_zlib_module(duk_context *ctx) {

  duk_idx_t idx = duk_push_object(ctx);

  duk_put_function_list(ctx, idx, zlib_fns);

  return 1;
}

int dk_register_module_zlib(struct duker_s *ctx) {

  dk_add_module_fn(ctx, "zlib", initialize_zlib_module);

  return 1;
}
#include "zlib.h"
#include "gzip.h"
#include <stdbool.h>
#include <string.h>
#include <zlib.h>

static int compress_buffer(unsigned char *dest, size_t dlen, unsigned char *src,
                           unsigned int slen, bool gzip, int level,
                           bool use_deflate) {

  z_stream stream;
  int bts = 0, ret = 0; //, dlen = 0;
  memset(&stream, 0, sizeof(z_stream));
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;

  int wndbits = 15 + (gzip ? 16 : 0);

  if (use_deflate) {
    if (Z_OK != deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                             wndbits, 8, Z_DEFAULT_STRATEGY)) {
      return 0;
    }
  } else {
    if (Z_OK != inflateInit2(&stream, wndbits))
      return 0;
  }

  stream.next_in = src;
  stream.avail_in = slen;

  stream.next_out = dest;
  stream.avail_out = dlen;
  int err;

  if (use_deflate) {
    if ((err = deflate(&stream, Z_FINISH)) != Z_STREAM_END) {
      deflateEnd(&stream);
      return 0;
    }
  } else {
    if ((err = inflate(&stream, Z_FINISH)) != Z_STREAM_END) {

      inflateEnd(&stream);
      return 0;
    }
  }

  ret = stream.total_out;
  if (use_deflate)
    deflateEnd(&stream);
  else
    inflateEnd(&stream);
  return ret;
}

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

static duk_ret_t zlib_compress_buffer(duk_context *ctx, bool gzip) {
  const char *input = NULL;
  size_t len = 0;

  if (!(input = duk_str_or_buffer(ctx, 0, &len))) {
    duk_type_error(ctx, "invalid argument");
  }

  len++;

  size_t clen = compressBound(len) + 18;
  unsigned char *buffer = duk_push_dynamic_buffer(ctx, clen);
  int status = compress_buffer(buffer, clen, input, len, gzip,
                               Z_DEFAULT_COMPRESSION, true);
  if (!status) {
    duk_type_error(ctx, "could not %d", status);
  }

  clen = status;

  duk_resize_buffer(ctx, -1, clen);

  return 1;
}

static duk_ret_t zlib_decompress_buffer(duk_context *ctx, bool gzip) {
  const char *input = NULL;
  size_t len = 0;

  if (!(input = duk_str_or_buffer(ctx, 0, &len))) {
    duk_type_error(ctx, "invalid argument");
  }

  len++;

  size_t clen = len * 4; // compressBound(len) + 18;
  unsigned char *buffer = duk_push_dynamic_buffer(ctx, clen);
  int status = compress_buffer(buffer, clen, input, len, gzip,
                               Z_DEFAULT_COMPRESSION, false);
  if (!status) {
    duk_type_error(ctx, "could not decompress %d", status);
  }

  clen = status;

  duk_resize_buffer(ctx, -1, clen);

  return 1;
}

static duk_ret_t zlib_deflate_buffer(duk_context *ctx) {
  return zlib_compress_buffer(ctx, false);
}

static duk_ret_t zlib_inflate_buffer(duk_context *ctx) {
  return zlib_decompress_buffer(ctx, false);
}

static duk_ret_t zlib_gzip_buffer(duk_context *ctx) {
  return zlib_compress_buffer(ctx, true);
}

static duk_ret_t zlib_unzip_buffer(duk_context *ctx) {
  return zlib_decompress_buffer(ctx, true);
}

const duk_function_list_entry zlib_fns[] = {
    {"createDeflate", zlib_create_deflate, 1},
    {"gzip", zlib_gzip_buffer, 1},
    {"unzip", zlib_unzip_buffer, 1},
    {"deflate", zlib_deflate_buffer, 1},
    {"inflate", zlib_inflate_buffer, 1},
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
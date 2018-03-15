#include "crypto.h"
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>

enum hash_type {
  HASH_SHA1 = 1,
  HASH_SHA224,
  HASH_SHA256,
  HASH_SHA384,
  HASH_SHA512
};

static const char *str_or_buffer(duk_context *ctx, duk_idx_t idx, size_t *len) {
  if (duk_is_string(ctx, idx)) {
    const char *out = duk_require_string(ctx, idx);
    *len = strlen(out);
    return out;
  } else if (duk_is_buffer(ctx, idx)) {
    return duk_get_buffer(ctx, idx, (duk_size_t *)len);
  }
  return NULL;
}

static enum hash_type string_to_hash_type(const char *algo) {
  if (strcmp(algo, "sha") == 0) {
    return HASH_SHA1;
  } else if (strcmp(algo, "sha224") == 0) {
    return HASH_SHA224;
  } else if (strcmp(algo, "sha256") == 0) {
    return HASH_SHA256;
  } else if (strcmp(algo, "sha384") == 0) {
    return HASH_SHA384;
  } else if (strcmp(algo, "sha512") == 0) {
    return HASH_SHA512;
  }
  return 0;
}

typedef struct dukext_crypto_hash_s {
  enum hash_type type;
  void *handle;
} dukext_crypto_hash_t;

static dukext_crypto_hash_t *hash_instance(duk_context *ctx) {
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1,
                      "\xff"
                      "hash");
  dukext_crypto_hash_t *feature =
      (dukext_crypto_hash_t *)(duk_get_buffer(ctx, -1, NULL));
  duk_pop(ctx);
  return feature;
}

static duk_ret_t crypto_create_hash_update(duk_context *ctx) {
  dukext_crypto_hash_t *hash = hash_instance(ctx);

  const char *data = NULL;
  size_t size = 0;
  if (!(data = str_or_buffer(ctx, 0, &size))) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "should be buffer or string");
  }

  switch (hash->type) {
  case HASH_SHA1:
    mbedtls_sha1_update(hash->handle, (const unsigned char *)data, size);
    break;
  case HASH_SHA224:
  case HASH_SHA256:
    mbedtls_sha256_update(hash->handle, (const unsigned char *)data, size);
    break;
  case HASH_SHA384:
  case HASH_SHA512:
    mbedtls_sha256_update(hash->handle, (const unsigned char *)data, size);
    break;
  }

  duk_push_this(ctx);

  return 1;
}

static duk_ret_t crypto_create_hash_sum(duk_context *ctx) {

  dukext_crypto_hash_t *hash = hash_instance(ctx);
  unsigned char *ptr;
  switch (hash->type) {
  case HASH_SHA1:
    ptr = duk_push_fixed_buffer(ctx, sizeof(char) * 20);
    mbedtls_sha1_finish(hash->handle, ptr);
    mbedtls_sha1_starts(hash->handle);
    break;
  case HASH_SHA224:
  case HASH_SHA256:
    ptr = duk_push_fixed_buffer(ctx, sizeof(char) * 32);
    mbedtls_sha256_finish(hash->handle, ptr);
    mbedtls_sha256_starts(hash->handle, HASH_SHA224 == hash->type);
    break;
  case HASH_SHA384:
  case HASH_SHA512:
    ptr = duk_push_fixed_buffer(ctx, sizeof(char) * 64);
    mbedtls_sha512_finish(hash->handle, ptr);
    mbedtls_sha512_starts(hash->handle, HASH_SHA384 == hash->type);
    break;
  }

  return 1;
}
static duk_ret_t crypto_create_hash_finalize(duk_context *ctx) {

  duk_get_prop_string(ctx, 0,
                      "\xff"
                      "hash");

  dukext_crypto_hash_t *handle = duk_get_buffer(ctx, -1, NULL);
  switch (handle->type) {
  case HASH_SHA1:
    mbedtls_sha1_free(handle->handle);
    break;
  case HASH_SHA224:
  case HASH_SHA256:
    mbedtls_sha256_free(handle->handle);
    break;
  case HASH_SHA384:
  case HASH_SHA512:
    mbedtls_sha512_free(handle->handle);
    break;
  }
  free(handle->handle);
  return 0;
}

const duk_function_list_entry crypto_hash_fns[] = {
    {"update", crypto_create_hash_update, 1},
    {"digest", crypto_create_hash_sum, 1},
    {NULL}};

static duk_ret_t crypto_create_hash(duk_context *ctx) {

  const char *algo = duk_require_string(ctx, 0);

  duk_idx_t idx = duk_push_object(ctx);

  duk_put_function_list(ctx, idx, crypto_hash_fns);
  duk_push_c_function(ctx, crypto_create_hash_finalize, 1);
  duk_set_finalizer(ctx, idx);

  dukext_crypto_hash_t *handle;
  handle = duk_push_fixed_buffer(ctx, sizeof(handle));
  handle->handle = NULL;

  handle->type = string_to_hash_type(algo);

  switch (handle->type) {
  case HASH_SHA1:
    handle->handle = malloc(sizeof(mbedtls_sha1_context));
    mbedtls_sha1_init(handle->handle);
    mbedtls_sha1_starts(handle->handle);
    break;
  case HASH_SHA224:
  case HASH_SHA256:
    handle->handle = malloc(sizeof(mbedtls_sha256_context));
    mbedtls_sha256_init(handle->handle);
    mbedtls_sha256_starts(handle->handle, handle->type == HASH_SHA224);
    break;
  case HASH_SHA384:
  case HASH_SHA512:
    handle->handle = malloc(sizeof(mbedtls_sha512_context));
    mbedtls_sha512_init(handle->handle);
    mbedtls_sha512_starts(handle->handle, handle->type == HASH_SHA384);
    break;
  default:
    duk_error(ctx, 200, "crypto: unknown algorithm: %s", algo);
  }

  duk_put_prop_string(ctx, idx,
                      "\xff"
                      "hash");

  return 1;
fail:
  duk_error(ctx, 200, "crypto: unknown algorithm: %s", algo);
}

const duk_function_list_entry crypto_fns[] = {
    {"createHash", crypto_create_hash, 1}, {NULL}};

static duk_ret_t initialize_crypto_module(duk_context *ctx) {

  duk_idx_t idx = duk_push_object(ctx);

  duk_put_function_list(ctx, idx, crypto_fns);

  return 1;
}

int dk_register_module_crypto(struct duker_s *ctx) {
  dk_add_module_fn(ctx, "crypto", initialize_crypto_module);
  return 1;
}
#include "crypto.h"
#include <openssl/sha.h>

enum hash_type { HASH_SHA, HASH_SHA256 };

typedef struct dukext_crypto_hash_s {
  enum hash_type type;
  void *handle;
  // int hash_ref;
  int ref;
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
  if (duk_is_string(ctx, 0)) {
    data = duk_require_string(ctx, 0);
    size = strlen(data);
  }

  switch (hash->type) {
  case HASH_SHA:
    return SHA1_Update(hash->handle, data, size);
  case HASH_SHA256:
    return SHA256_Update(hash->handle, data, size);
    /*case DUKEXT_CRYPTO_HASH_SHA256:
      return SHA256_Update(handle->hash, data, size);
    case DUKEXT_CRYPTO_HASH_SHA512:
      return SHA512_Update(handle->hash, data, size);*/
  }

  return 0;
}
static duk_ret_t crypto_create_hash_sum(duk_context *ctx) { return 0; }
static duk_ret_t crypto_create_hash_finalize(duk_context *ctx) {

  duk_get_prop_string(ctx, 0,
                      "\xff"
                      "hash");

  dukext_crypto_hash_t *handle = duk_get_buffer(ctx, -1, NULL);
  switch (handle->type) {
  case HASH_SHA:
    free(handle->handle);
    break;
  case HASH_SHA256:
    free(handle->handle);
    break;
  }
  // free(handle);
  return 0;
}

const duk_function_list_entry crypto_hash_fns[] = {
    {"update", crypto_create_hash_update, 1},
    {"sum", crypto_create_hash_sum, 1},
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

  if (strcmp(algo, "sha") == 0) {
    handle->handle = malloc(sizeof(SHA_CTX));
    handle->type = HASH_SHA;
    if (!SHA1_Init(handle->handle)) {
      goto fail;
    }

  } else if (strcmp(algo, "sha256") == 0) {
    handle->handle = malloc(sizeof(SHA256_CTX));
    handle->type = HASH_SHA256;
    if (!SHA256_Init(handle->handle)) {
      goto fail;
    }

  } /*else if (strcmp(algo, "sha512") == 0) {
    handle->hash = malloc(sizeof(SHA512_CTX));
    handle->type = DUKEXT_CRYPTO_HASH_SHA512;
    if (!SHA512_Init(handle->hash)) {
      goto fail;
    }

  } else if (strcmp(algo, "md5") == 0) {
    handle->hash = malloc(sizeof(MD5_CTX));
    handle->type = DUKEXT_CRYPTO_HASH_MD5;
    if (!MD5_Init(handle->hash)) {
      goto fail;
    }

  }*/ else {
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
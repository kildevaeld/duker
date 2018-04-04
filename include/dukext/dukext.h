#pragma once
#include <duktape.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

#define DUKEXT_SANDBOX_MIN 128

typedef struct dukext_s dukext_t;

typedef enum dukext_log_level_s { DEBUG, INFO, WARN, ERROR } dukext_log_level_t;

typedef void (*dukext_console_logger)(duk_context *ctx,
                                      dukext_log_level_t level,
                                      const char *message, dukext_t *vm);

typedef bool (*dukext_module_resolver)(duk_context *ctx, const char *filename);

typedef duk_ret_t (*dukext_module_initializer)(duk_context *ctx);

typedef enum {
  DUKEXT_FN_TYPE = 1 << 0,
  DUKEXT_STR_TYPE = 1 << 1,
  DUKEXT_LIB_TYPE = 1 << 2,
  DUKEXT_PATH_TYPE = 1 << 3
} dukext_module_type;

typedef struct {
  char *message;
} dukext_err_t;

typedef struct {
  dukext_module_resolver resolver;
  // Logging handler
  dukext_console_logger logger;
  int module_types;
  duk_size_t max_heap;
} dukext_config_t;

dukext_t *dukext_create_default();
dukext_t *dukext_create(dukext_config_t config);
void dukext_destroy(dukext_t *);

void dukext_config_init(dukext_config_t *cfg);

void dukext_err_free(dukext_err_t *err);

// Properties
duk_context *dukext_get_ctx(dukext_t *);

duk_ret_t dukext_eval_path(dukext_t *vm, const char *path, dukext_err_t **err);
duk_ret_t dukext_eval_script(dukext_t *vm, const char *script,
                             const char *path);

#ifdef __cplusplus
}
#endif
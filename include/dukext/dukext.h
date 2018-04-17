#pragma once
#include <duktape.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUKEXT_SANDBOX_MIN 256 * 1024

typedef struct dukext_s dukext_t;

typedef enum dukext_log_level_s { DEBUG, INFO, WARN, ERROR } dukext_log_level_t;

typedef void (*dukext_console_logger)(duk_context *ctx,
                                      dukext_log_level_t level,
                                      const char *message, dukext_t *vm);

typedef duk_ret_t (*dukext_module_initializer)(duk_context *ctx);

typedef enum { DUKEXT_FILE_TYPE = 1 << 0 } dukext_module_type;

typedef enum {
  DUKEXT_MOD_FILE = 1 << 0,
  DUKEXT_MOD_PROMPT = 1 << 1,
} dukext_buildins;

typedef struct {
  char *message;
} dukext_err_t;

typedef struct {
  // Logging handler
  dukext_console_logger logger;
  int module_types;
  int modules;
  duk_size_t max_heap;
} dukext_config_t;

typedef struct {
  int heap;
  int count;
} dukext_stat_t;

dukext_t *dukext_create_default();
dukext_t *dukext_create(dukext_config_t config);
void dukext_destroy(dukext_t *);

void dukext_config_init(dukext_config_t *cfg);

void dukext_err_free(dukext_err_t *err);

void dukext_dump_stats(dukext_t *vm);

// Properties
duk_context *dukext_get_ctx(dukext_t *);
dukext_config_t dukext_get_config(dukext_t *);

duk_ret_t dukext_eval_path(dukext_t *vm, const char *path, dukext_err_t **err);
duk_ret_t dukext_eval_script(dukext_t *vm, const char *script, const char *path,
                             dukext_err_t **err);

#ifdef __cplusplus
}
#endif
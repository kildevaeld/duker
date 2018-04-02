#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <duktape.h>
#include <stdbool.h>

typedef struct duker_s duker_t;

typedef enum dukext_log_level_s { DEBUG, INFO, WARN, ERROR } dukext_log_level_t;

typedef void (*dukext_console_logger)(duk_context *ctx,
                                      dukext_log_level_t level,
                                      const char *message, duker_t *vm);

typedef bool (*dukext_module_resolver)(duk_context *ctx, const char *filename);

typedef struct duker_config_s {
  dukext_module_resolver resolver;
  // Logging handler
  dukext_console_logger logger;
  int module_types;
} duker_config_t;

typedef struct duker_err_s {
  char *message;
} duker_err_t;

duker_t *dukext_create(duk_context *ctx);
void dukext_free(duker_t *);

duk_context *dukext_duk_context(duker_t *);
duker_config_t *dukext_get_config(duker_t *);

/**
 * Evaluate a path
 */
duk_ret_t dukext_eval_path(duker_t *, const char *, duker_err_t **err);

/**
 * Evaluate script
 */
duk_ret_t dukext_eval_script(duker_t *, const char *path, const char *script,
                             duker_err_t **err);
void dukext_free_err(duker_err_t *);

int dukext_add_module_fn(duker_t *ctx, const char *name,
                         duk_ret_t (*fn)(duk_context *));
int dukext_add_module_str(duker_t *ctx, const char *name, const char *content);

void dukext_dump_context_stdout(duk_context *ctx);

void dukext_add_default_modules(duker_t *ctx);

void dukext_stash_set_ptr(duk_context *ctx, const char *name, void *ptr);
void *dukext_stash_get_ptr(duk_context *ctx, const char *name);
void dukext_stash_rm_ptr(duk_context *ctx, const char *name);

#ifdef __cplusplus
}
#endif

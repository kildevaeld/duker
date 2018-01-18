#pragma once
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct duker_s duker_t;

typedef struct duker_config_s {

} duker_config_t;

typedef struct duker_err_s {
  char *message;
} duker_err_t;

duker_t *dk_create(duk_context *ctx);
void dk_free(duker_t *);

duk_context *dk_duk_context(duker_t *);

duk_ret_t dk_eval_path(duker_t *, const char *, duker_err_t **err);
duk_ret_t dk_eval_script(duker_t *, const char *path, const char *script);
void dk_free_err(duker_err_t *);
int dk_add_module_fn(duker_t *ctx, const char *name,
                     duk_ret_t (*fn)(duk_context *));
int dk_add_module_str(duker_t *ctx, const char *name, const char *content);

void dk_dump_context_stdout(duk_context *ctx);

void dk_add_default_modules(duker_t *ctx);

void dk_stash_set_ptr(duk_context *ctx, const char *name, void *ptr);
void *dk_stash_get_ptr(duk_context *ctx, const char *name);
void *dk_stash_rm_ptr(duk_context *ctx, const char *name);

#ifdef __cplusplus
}
#endif

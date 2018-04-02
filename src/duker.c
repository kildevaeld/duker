#include "log.h"
#include "module.h"
#include "type.h"
#include <csystem/file.h>
#include <csystem/path.h>
#include <duker/duker.h>
#include <duker/refs.h>
#include <duktape.h>
#include <stdlib.h>

// Modules
//#include "extras/crypto.h"

#if defined(DUKER_USE_HTTP)
#include "extras/http.h"
#endif

#include "extras/zlib.h"
#include "modules/fs.h"
#include "modules/path.h"
// builtins
#include "builtins/duker_module.h"
#include "builtins/process.h"
#include "duker_console.h"

#include "event-emitter.h"

duker_t *dukext_create(duk_context *ctx) {

  duker_t *d = malloc(sizeof(duker_t));
  if (!d)
    return NULL;
  // No duk context supplied, create one.
  if (!ctx) {
    d->ctx = duk_create_heap_default();
    if (!d->ctx) {
      free(d);
      return NULL;
    }
    d->_c = 1;
  } else {
    d->ctx = ctx;
  }
  d->config.logger = NULL;
  d->config.resolver = NULL;

  d->modules = NULL;

  dukext_stash_set_ptr(d->ctx, "duker", d);

  dukext_ref_setup(d->ctx);

  duk_console_init(d->ctx, DUK_CONSOLE_PROXY_WRAPPER);
  dukext_module_process_init(d);
  init_modules(d->ctx);

  return d;
}

void dukext_free(duker_t *d) {
  if (!d)
    return;

  if (d->_m) {
#if defined(DUKER_USE_HTTP)
    dukext_unregister_module_http(d);
#endif
  }

  free_modules(d);
  dukext_stash_rm_ptr(d->ctx, "duker");
  if (d->_c)
    duk_destroy_heap(d->ctx);
  free(d);
}

duker_config_t *dukext_get_config(duker_t *ctx) { return &ctx->config; }

void dukext_free_err(duker_err_t *err) {
  if (!err)
    return;
  free(err->message);
  free(err);
}

duk_context *dukext_duk_context(duker_t *ctx) { return ctx->ctx; }

duk_ret_t dukext_eval_path(duker_t *ctx, const char *path, duker_err_t **err) {
  char *buffer = NULL;
  int len = 0;
  int c = 0;

  if (!cs_path_is_abs(path)) {
    path = cs_path_abs(path, NULL, 0);
    c = 1;
  }

  if (!(buffer = cs_read_file(path, NULL, 0, &len))) {
    if (c)
      free((char *)path);
    if (err) {
      duker_err_t *e = (duker_err_t *)malloc(sizeof(duker_err_t));
      e->message = strdup("file not found");
      *err = e;
    }
    return DUK_EXEC_ERROR;
  }

  duk_push_lstring(ctx->ctx, buffer, len);
  duk_ret_t ret = duk_module_node_peval_main(ctx->ctx, path);

  free(buffer);
  if (c)
    free((char *)path);

  if (ret == DUK_EXEC_ERROR && err) {
    if (duk_get_prop_string(ctx->ctx, -1, "stack")) {
      duk_replace(ctx->ctx, -2);
    } else {
      duk_pop(ctx->ctx);
    }
    duker_err_t *e = (duker_err_t *)malloc(sizeof(duker_err_t));
    e->message = strdup(duk_require_string(ctx->ctx, -1));
    *err = e;
  }

  return ret;
}

duk_ret_t dukext_eval_script(duker_t *ctx, const char *path, const char *script,
                             duker_err_t **err) {

  duk_eval_string(ctx->ctx, script);
  duk_ret_t ret = duk_module_node_peval_main(ctx->ctx, path);
  if (ret == DUK_EXEC_ERROR && err) {
    if (duk_get_prop_string(ctx->ctx, -1, "stack")) {
      duk_replace(ctx->ctx, -2);
    } else {
      duk_pop(ctx->ctx);
    }
    duker_err_t *e = (duker_err_t *)malloc(sizeof(duker_err_t));
    e->message = strdup(duk_require_string(ctx->ctx, -1));
    *err = e;
  }

  return ret;
}

int dukext_add_module_fn(duker_t *ctx, const char *name,
                         duk_ret_t (*fn)(duk_context *)) {

  return add_module_fn(ctx, name, fn);
}
int dukext_add_module_str(duker_t *ctx, const char *name, const char *content) {
  return add_module_str(ctx, name, content);
}

void dukext_dump_context_stdout(duk_context *ctx) {
  duk_push_context_dump(ctx);
  printf("%s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);
}

void dukext_add_default_modules(duker_t *ctx) {
  ctx->_m = 1;
  dukext_register_module_path(ctx);
  dukext_register_module_fs(ctx);
  // dukext_register_module_crypto(ctx);
  dukext_register_module_zlib(ctx);

  dukext_register_module_event_emitter(ctx);

#if defined(DUKER_USE_HTTP)
  dukext_register_module_http(ctx);
#endif
}

void dukext_stash_set_ptr(duk_context *ctx, const char *name, void *ptr) {
  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop(ctx);
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "globals");
  }

  duk_push_pointer(ctx, ptr);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);
}
void *dukext_stash_get_ptr(duk_context *ctx, const char *name) {

  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop_2(ctx);
    return NULL;
  }

  duk_get_prop_string(ctx, -1, name);
  if (duk_is_null_or_undefined(ctx, -1)) {
    return NULL;
  }
  void *c = duk_to_pointer(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

void dukext_stash_rm_ptr(duk_context *ctx, const char *name) {
  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop_2(ctx);
    return;
  }

  duk_del_prop_string(ctx, -1, name);
  duk_pop_2(ctx);
}
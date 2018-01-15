#include "log.c"
#include "module.h"
#include "type.h"
#include <csystem/file.h>
#include <csystem/path.h>
#include <duker/duker.h>
#include <duktape.h>
#include <stdlib.h>
// Modules
#include "modules/fs.h"
#include "modules/path.h"
// builtins
#include "builtins/duker_console.h"
#include "builtins/duker_module.h"
#include "builtins/process.h"

#include <duker/refs.h>

duker_t *dk_create(duk_context *ctx) {

  duker_t *d = malloc(sizeof(duker_t));
  if (!d)
    return NULL;

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

  d->modules = NULL;

  // Save duker_t in global stash
  duk_push_global_stash(d->ctx);
  duk_push_pointer(d->ctx, (void *)d);
  duk_put_prop_string(d->ctx, -2, "duker");
  duk_pop(d->ctx);

  dk_ref_setup(d->ctx);

  duk_console_init(d->ctx, DUK_CONSOLE_PROXY_WRAPPER);
  dk_module_process_init(d);
  init_modules(d->ctx);

  return d;
}

void dk_free(duker_t *d) {
  if (!d)
    return;
  free_modules(d);
  if (d->_c)
    duk_destroy_heap(d->ctx);
  free(d);
}

void dk_free_err(duker_err_t *err) {
  if (!err)
    return;
  free(err->message);
  free(err);
}
duk_context *dk_duk_context(duker_t *ctx) { return ctx->ctx; }

duk_ret_t dk_eval_path(duker_t *ctx, const char *path, duker_err_t **err) {
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

duk_ret_t dk_eval_script(duker_t *ctx, const char *path, const char *script) {

  duk_eval_string(ctx->ctx, script);
  return duk_module_node_peval_main(ctx->ctx, path);
}

int dk_add_module_fn(duker_t *ctx, const char *name,
                     duk_ret_t (*fn)(duk_context *)) {

  return add_module_fn(ctx, name, fn);
}
int dk_add_module_str(duker_t *ctx, const char *name, const char *content) {
  return add_module_str(ctx, name, content);
}

void dk_dump_context_stdout(duk_context *ctx) {
  duk_push_context_dump(ctx);

  // fprintf(stdout, "%s\n", duk_safe_to_string(ctx, -1));
  log_debug("%s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);
}

void dk_add_default_modules(duker_t *ctx) {
  dk_register_module_path(ctx);
  dk_register_module_fs(ctx);
}

void dk_stash_set_ptr(duk_context *ctx, const char *name, void *ptr) {
  // Save duker_t in global stash
  duk_push_global_stash(ctx);
  duk_push_pointer(ctx, ptr);
  duk_put_prop_string(ctx, -2, name);
  duk_pop(ctx);
}
void *dk_stash_get_ptr(duk_context *ctx, const char *name) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, name);
  void *c = duk_to_pointer(ctx, -1);
  duk_pop_2(ctx);
  return c;
}
#include "commonjs.h"
#include "definitions.h"
#include "sandbox.h"
#include <csystem/file.h>
#include <csystem/path.h>
#include <dukext/dukext.h>
#include <dukext/utils.h>
#include "console.h"

dukext_t *dukext_create_default() {
  dukext_config_t cfg;
  dukext_config_init(&cfg);
  return dukext_create(cfg);
}

dukext_t *dukext_create(dukext_config_t config) {
  struct dukext_s *vm = malloc(sizeof(struct dukext_s));

  if (!vm)
    return NULL;

  vm->config = config;
  vm->modules = NULL;

  if (vm->config.max_heap >= DUKEXT_SANDBOX_MIN) {
    vm->ctx = duk_create_heap(sandbox_alloc, sandbox_realloc, sandbox_free, vm,
                              sandbox_fatal);
  } else {
    vm->ctx = duk_create_heap_default();
  }

  if (!vm->ctx) {
    free(vm);
    return NULL;
  }

  duk_stash_set_ptr(vm->ctx, "dukext_vm", vm);

  dukextp_init_commonjs(vm);
  duk_console_init(vm->ctx, DUK_CONSOLE_PROXY_WRAPPER);

  return vm;
}

void dukext_destroy(dukext_t *vm) {
  if (vm == NULL)
    return;

  if (vm->modules) {
  }

  duk_destroy_heap(vm->ctx);
  vm->ctx = NULL;
  free(vm);
}

void dukext_config_init(dukext_config_t *cfg) {
  cfg->logger = NULL;
  cfg->max_heap = 0;
  cfg->module_types =
      DUKEXT_FN_TYPE | DUKEXT_PATH_TYPE | DUKEXT_LIB_TYPE | DUKEXT_STR_TYPE;
  cfg->resolver = NULL;
}

void dukext_err_free(dukext_err_t *err) {
  if (!err)
    return;

  if (err->message)
    free(err->message);
  free(err);
}

duk_context *dukext_get_ctx(dukext_t *vm) { return vm->ctx; }

duk_ret_t dukext_eval_path(dukext_t *vm, const char *path, dukext_err_t **err) {
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
      dukext_err_t *e = (dukext_err_t *)malloc(sizeof(dukext_err_t));
      e->message = strdup("file not found");
      *err = e;
    }
    return DUK_EXEC_ERROR;
  }

  duk_context *ctx = vm->ctx;

  duk_push_lstring(ctx, buffer, len);
  duk_ret_t ret = dukextp_commonjs_eval_main(ctx, path);

  free(buffer);
  if (c)
    free((char *)path);

  if (ret == DUK_EXEC_ERROR && err) {
    if (duk_get_prop_string(ctx, -1, "stack")) {
      duk_replace(ctx, -2);
    } else {
      duk_pop(ctx);
    }
    dukext_err_t *e = (dukext_err_t *)malloc(sizeof(dukext_err_t));
    e->message = strdup(duk_require_string(ctx, -1));
    *err = e;
  }

  return ret;
}
duk_ret_t dukext_eval_script(dukext_t *vm, const char *script,
                             const char *path) {}
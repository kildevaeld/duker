#include "commonjs.h"
#include "commonjs_file.h"
#include "commonjs_module.h"
#include "console.h"
#include "definitions.h"
#include "sandbox.h"
#include <csystem/file.h>
#include <csystem/path.h>
#include <dukext/dukext.h>
#include <dukext/module.h>
#include <dukext/utils.h>

dukext_t *dukext_create_default() {
  dukext_config_t cfg;
  dukext_config_init(&cfg);
  cfg.module_types = DUKEXT_FILE_TYPE;
  return dukext_create(cfg);
}

// ([a-zA-Z]+)(?:\:\/\/)(\/?[a-zA-Z\.\-]+(?:\/[a-zA-Z\.\-]+)*)
// ^(?:\/|\.\.?\/)(?:[^\/\0]+(?:\/)?)+$

static duk_ret_t get_module_resolver(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "resolvers");
  duk_get_prop_string(ctx, -1, duk_require_string(ctx, 0));

  return 1;
}

static void init_stash(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_eval_string(ctx,
                  "({"
                  "protocol: "
                  "/^([a-zA-Z0-9]+)(?:\\:\\/\\/)(\\/?[a-zA-Z0-9\\.\\-]+(?:\\/"
                  "[a-zA-Z0-9\\.\\-]+)*)$/,"
                  "file: /^(?:\\/|\\.\\.?\\/)(?:[^\\/\\0]+(?:\\/)?)+$/"
                  "})");
  duk_put_prop_string(ctx, -2, "constants");

  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "modules");

  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "resolvers");
  duk_push_c_lightfunc(ctx, get_module_resolver, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "find_resolver");

  duk_push_c_lightfunc(ctx, dukextp_module_push, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "find_module");

  duk_pop(ctx);
}

dukext_t *dukext_create(dukext_config_t config) {
  struct dukext_s *vm = malloc(sizeof(struct dukext_s));

  if (!vm)
    return NULL;

  vm->config = config;
  vm->modules = NULL;
  vm->stats.count = 0;
  vm->stats.heap = 0;

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
  init_stash(vm->ctx);
  duk_ref_setup(vm->ctx);

  dukextp_init_commonjs(vm);
  duk_console_init(vm->ctx, DUK_CONSOLE_PROXY_WRAPPER);

  dukext_set_module_resolver(vm, "module", cjs_resolve_module, cjs_load_module);

  if (config.module_types & DUKEXT_FILE_TYPE) {
    dukext_set_module_resolver(vm, "file", cjs_resolve_file, cjs_load_file);
  }

  return vm;
}

void dukext_dump_stats(dukext_t *vm) {
  printf("heap %i, count %i\n", vm->stats.heap, vm->stats.count);
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
  cfg->module_types = 0;
  // cfg->resolver = NULL;
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

  duk_context *ctx = vm->ctx;

  int size = cs_file_size(path);
  buffer = duk_push_fixed_buffer(ctx, size);
  if (!(buffer = cs_read_file(path, buffer, size, &len))) {

    if (c)
      free((char *)path);
    if (err) {
      dukext_err_t *e = (dukext_err_t *)malloc(sizeof(dukext_err_t));
      e->message = strdup("file not found");
      *err = e;
    }
    return DUK_EXEC_ERROR;
  }

  duk_buffer_to_string(ctx, -1);

  duk_ret_t ret = dukextp_commonjs_eval_main(ctx, path);

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
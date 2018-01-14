#include "module.h"
#include "builtins/duker_module.h"
#include "log.h"
#include "type.h"
#include <csystem/file.h>
#include <csystem/path.h>
#include <csystem/standardpaths.h>

static const char *get_main(duk_context *ctx) {

  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1,
                      "\xff"
                      "mainModule");

  duk_get_prop_string(ctx, -1, "filename");
  const char *c = duk_get_string(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

static duker_t *get_duker(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "duker");
  duker_t *c = (duker_t *)duk_to_pointer(ctx, -1);
  duk_pop(ctx);
  return c;
}

static duk_ret_t cb_resolve_module(duk_context *ctx) {
  const char *module_id;
  const char *parent_id;

  module_id = duk_require_string(ctx, 0);
  parent_id = duk_require_string(ctx, 1);

  duker_t *vm = get_duker(ctx);
  struct modules_bag_s *bag = get_module(vm, module_id);

  if (bag) {
    duk_push_string(ctx, module_id);
  } else {
    // file
    if (strncmp(module_id, "/", 1) == 0 || strncmp(module_id, "./", 2) == 0) {
      if (strlen(parent_id) == 0) {
        parent_id = get_main(ctx);
      }

      int dl = cs_path_dir(parent_id);
      char parent_path[dl + 1];
      strncpy(parent_path, parent_id, dl);
      parent_path[dl] = '\0';

      char *full_file = cs_path_join(NULL, parent_path, module_id, NULL);

      int idx = -1;
      int len = cs_path_ext(full_file, &idx);

      if (len == 0) {
        duk_push_sprintf(ctx, "%s.js", full_file);
      } else {
        duk_push_string(ctx, full_file);
      }

      free(full_file);

    } else {
      return 0;
    }
  }

  return 1;
}

static duk_ret_t cb_load_module(duk_context *ctx) {
  const char *filename;
  const char *module_id;

  module_id = duk_require_string(ctx, 0);
  duk_get_prop_string(ctx, 2, "filename");
  filename = duk_require_string(ctx, -1);

  duker_t *vm = get_duker(ctx);

  struct modules_bag_s *bag = get_module(vm, filename);

  if (bag != NULL) {
    if (bag->type == FN_MODTYPE) {
      duk_push_c_function(ctx, bag->module.func, 0);
    } else {
      duk_push_string(ctx, bag->module.script);
    }
  } else {

    if (!cs_file_exists(filename)) {
      goto fail;
    }

    int size = 0;
    char *buffer = cs_read_file(filename, NULL, 0, &size);
    if (!buffer) {
      goto fail;
    }

    duk_push_lstring(ctx, buffer, size);
    free(buffer);
  }

  return 1;

fail:
  log_debug("module: '%s' - %s not found", filename, module_id);
  return duk_type_error(ctx, "cannot find module: %s", module_id);
}

void init_modules(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "load");
  duk_module_node_init(ctx);
}

int add_module_fn(struct duker_s *ctx, const char *n,
                  duk_ret_t (*fn)(duk_context *)) {

  struct modules_bag_s *m;

  HASH_FIND_STR(ctx->modules, n, m);
  if (m != NULL) {
    return 1;
  }

  m = (struct modules_bag_s *)malloc(sizeof(struct modules_bag_s));
  m->name = (char *)malloc(sizeof(char) * strlen(n) + 1);
  strcpy(m->name, n);

  m->type = FN_MODTYPE;
  m->module.func = fn;
  log_debug("registered module %s", n);
  HASH_ADD_STR(ctx->modules, name, m);

  return 0;
}

int add_module_str(struct duker_s *ctx, const char *name, const char *content) {
  struct modules_bag_s *m;
  HASH_FIND_STR(ctx->modules, name, m);
  if (m != NULL) {
    return 1;
  }

  m = (struct modules_bag_s *)malloc(sizeof(struct modules_bag_s));
  m->name = (char *)malloc(sizeof(char) * strlen(name) + 1);
  strcpy(m->name, name);

  m->module.script = (char *)malloc(sizeof(char) * strlen(content) + 1);
  strcpy(m->module.script, content);
  m->type = STR_MODTYPE;

  log_debug("registered module %s", name);
  HASH_ADD_STR(ctx->modules, name, m);
  return 0;
}

struct modules_bag_s *get_module(struct duker_s *ctx, const char *name) {
  struct modules_bag_s *s;
  HASH_FIND_STR(ctx->modules, name, s);
  return s;
}

void free_modules(duker_t *ctx) {
  if (ctx->modules == NULL)
    return;

  struct modules_bag_s *current, *tmp;

  HASH_ITER(hh, ctx->modules, current, tmp) {
    HASH_DEL(ctx->modules, current);
    if (current->type == STR_MODTYPE) {
      free(current->module.script);
    }
    free(current);
  }
}
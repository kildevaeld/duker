#include "module.h"
#include "log.h"
#include "type.h"
#include <csystem/file.h>
#include <csystem/path.h>
#include <csystem/standardpaths.h>
#include <duker/duker_module.h>

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

static int show_fn(duk_context *ctx) {
  printf("show");
  return 0;
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

  int idx;
  int len = cs_path_ext(module_id, &idx);
  if (len == 0) {
    duk_push_sprintf(ctx, "%s.js", module_id);
  } else {
    duk_push_string(ctx, module_id);
  }

  // duk_push_sprintf(ctx, "%s.js", module_id);
  // printf("resolve_cb: id:'%s', parent-id:'%s', resolve-to:'%s'\n", module_id,
  //     parent_id, duk_get_string(ctx, -1));

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

  if (bag == NULL) {
    log_debug("module: '%a 'not found", filename);
    return duk_type_error(ctx, "cannot find module: %s", module_id);
  }

  if (bag->type == FN_MODTYPE) {
    duk_push_c_function(ctx, bag->module.func, 0);
  } else {
    duk_push_string(ctx, bag->module.script);
  }

  return 1;
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

  m->type = STR_MODTYPE;
  m->module.script = content;
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
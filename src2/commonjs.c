#include "commonjs.h"
#include <csystem/features.h>
#include <csystem/file.h>
#include <csystem/path.h>
#include <dlfcn.h>
#include <dukext/module.h>
#include <dukext/utils.h>

#ifdef CS_PLATFORM_DARWIN
#define LIBRARY_EXT ".dylib"
#elif CS_PLATFORM_POSIX
#define LIBRARY_EXT ".so"
#endif

static duk_ret_t duk__resolve_module(duk_context *ctx, void *udata);
static duk_ret_t duk__load_module(duk_context *ctx, void *udata);

static void duk__push_module_object(duk_context *ctx, const char *id,
                                    duk_bool_t main);

static duk_bool_t duk__get_cached_module(duk_context *ctx, const char *id) {
  duk_push_global_stash(ctx);
  (void)duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("requireCache"));
  if (duk_get_prop_string(ctx, -1, id)) {
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    return 1;
  } else {
    duk_pop_3(ctx);
    return 0;
  }
}

/* Place a `module` object on the top of the value stack into the require
 * cache based on its `.id` property.  As a convenience to the caller, leave
 * the object on top of the value stack afterwards.
 */
static void duk__put_cached_module(duk_context *ctx) {
  /* [ ... module ] */

  duk_push_global_stash(ctx);
  (void)duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("requireCache"));
  duk_dup(ctx, -3);

  /* [ ... module stash req_cache module ] */

  (void)duk_get_prop_string(ctx, -1, "id");
  duk_dup(ctx, -2);
  duk_put_prop(ctx, -4);

  duk_pop_3(ctx); /* [ ... module ] */
}

static void duk__del_cached_module(duk_context *ctx, const char *id) {
  duk_push_global_stash(ctx);
  (void)duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("requireCache"));
  duk_del_prop_string(ctx, -1, id);
  duk_pop_2(ctx);
}

static duk_ret_t duk__handle_require(duk_context *ctx) {

  dukext_t *vm = duk_get_dukext(ctx);

  duk_push_current_function(ctx);
  (void)duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("moduleId"));
  const char *parent_id = duk_require_string(ctx, -1);
  (void)parent_id; /* not used directly; suppress warning */

  /* [ id require parent_id ] */

  const char *id = duk_require_string(ctx, 0);

  // duk_dup(ctx, 0);  /* module ID */
  // duk_dup(ctx, -2); /* parent ID */
  // dukext_dump_context_stdout(ctx);

  duk_ret_t ret = duk_safe_call(ctx, duk__resolve_module, vm, 3, 1);

  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }

  if (duk_is_undefined(ctx, -1)) {
    duk_type_error(ctx, "could resolve file %s", id);
  }

  if (!duk_is_object_coercible(ctx, -1)) {
    duk_type_error(ctx, "invalid return type");
  }

  duk_get_prop_string(ctx, -1, "id");
  // duk_get_prop_index(ctx, -1, 0);
  id = duk_require_string(ctx, -1);
  duk_pop(ctx);

  if (duk__get_cached_module(ctx, id)) {
    goto have_module; /* use the cached module */
  }

  duk__push_module_object(ctx, id, 0 /*main*/);
  duk__put_cached_module(ctx); /* module remains on stack */

  /*
   *  From here on out, we have to be careful not to throw.  If it can't be
   *  avoided, the error must be caught and the module removed from the
   *  require cache before rethrowing.  This allows the application to
   *  reattempt loading the module.
   */

  duk_idx_t module_idx = duk_normalize_index(ctx, -1);
  duk_idx_t info_idx = duk_normalize_index(ctx, -2);

  duk_get_prop_string(ctx, info_idx, "resolver");
  duk_get_prop_string(ctx, -1, "load");

  duk_dup(ctx, info_idx);
  duk_dup(ctx, module_idx);

  ret = duk_pcall(ctx, 2);

  if (ret != DUK_EXEC_SUCCESS) {
    duk__del_cached_module(ctx, id);
    duk_throw(ctx);
  }
  duk_pop_2(ctx);

  /* fall through */

have_module:
  /* [ ... module ] */

  (void)duk_get_prop_string(ctx, -1, "exports");
  return 1;
}

static void duk__push_require_function(duk_context *ctx, const char *id) {
  duk_push_c_function(ctx, duk__handle_require, 1);
  duk_push_string(ctx, "name");
  duk_push_string(ctx, "require");
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE);
  duk_push_string(ctx, id);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("moduleId"));

  /* require.cache */
  duk_push_global_stash(ctx);
  (void)duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("requireCache"));
  duk_put_prop_string(ctx, -3, "cache");
  duk_pop(ctx);

  /* require.main */
  duk_push_global_stash(ctx);
  (void)duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("mainModule"));
  duk_put_prop_string(ctx, -3, "main");
  duk_pop(ctx);
}

static void duk__push_module_object(duk_context *ctx, const char *id,
                                    duk_bool_t main) {
  duk_push_object(ctx);

  /* Set this as the main module, if requested */
  if (main) {
    duk_push_global_stash(ctx);
    duk_dup(ctx, -2);
    duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("mainModule"));
    duk_pop(ctx);
  }

  /* Node.js uses the canonicalized filename of a module for both module.id
   * and module.filename.  We have no concept of a file system here, so just
   * use the module ID for both values.
   */
  duk_push_string(ctx, id);
  duk_dup(ctx, -1);
  duk_put_prop_string(ctx, -3, "filename");
  duk_put_prop_string(ctx, -2, "id");

  /* module.exports = {} */
  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "exports");

  /* module.loaded = false */
  duk_push_false(ctx);
  duk_put_prop_string(ctx, -2, "loaded");

  /* module.require */
  duk__push_require_function(ctx, id);
  duk_put_prop_string(ctx, -2, "require");
}

static duk_int_t duk__eval_module_source(duk_context *ctx, void *udata) {

  const char *src;

  /*
   *  Stack: [ ... module source ]
   */

  (void)udata;

  /* Wrap the module code in a function expression.  This is the simplest
   * way to implement CommonJS closure semantics and matches the behavior of
   * e.g. Node.js.
   */
  duk_push_string(ctx,
                  "(function(exports,require,module,__filename,__dirname){");
  src = duk_require_string(ctx, -2);
  duk_push_string(
      ctx, (src[0] == '#' && src[1] == '!') ? "//" : ""); /* Shebang support. */
  duk_dup(ctx, -3);                                       /* source */
  duk_push_string(
      ctx,
      "\n})"); /* Newline allows module last line to contain a // comment. */
  duk_concat(ctx, 4);

  /* [ ... module source func_src ] */

  (void)duk_get_prop_string(ctx, -3, "filename");
  duk_compile(ctx, DUK_COMPILE_EVAL);
  duk_call(ctx, 0);

  /* [ ... module source func ] */

  /* Set name for the wrapper function. */
  duk_push_string(ctx, "name");
  duk_push_string(ctx, "main");
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_FORCE);

  /* call the function wrapper */
  (void)duk_get_prop_string(ctx, -3, "exports");  /* exports */
  (void)duk_get_prop_string(ctx, -4, "require");  /* require */
  duk_dup(ctx, -5);                               /* module */
  (void)duk_get_prop_string(ctx, -6, "filename"); /* __filename */
  duk_push_undefined(ctx);                        /* __dirname */
  duk_call(ctx, 5);

  /* [ ... module source result(ignore) ] */

  /* module.loaded = true */
  duk_push_true(ctx);
  duk_put_prop_string(ctx, -4, "loaded");

  /* [ ... module source retval ] */

  duk_pop_2(ctx);

  /* [ ... module ] */

  return 1;
}

static duk_ret_t match_reg(duk_context *ctx) {
  const char *re = duk_require_string(ctx, 0);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "constants");
  if (!duk_get_prop_string(ctx, -1, re)) {
    duk_type_error(ctx, "regex '%s' does not exists", re);
  }

  duk_push_string(ctx, "match");
  duk_dup(ctx, -2);
  duk_call_prop(ctx, 1, 1);
  return 1;
}

static bool require_type_check(duk_context *ctx, const char *type) {
  duk_push_c_lightfunc(ctx, match_reg, 2, 2, 0);
  duk_push_string(ctx, type);
  duk_dup(ctx, -3);

  duk_ret_t ret = duk_pcall(ctx, 2);
  if (ret != DUK_EXEC_SUCCESS) {
    return false;
  }
  bool is = !duk_is_null(ctx, -1);

  duk_pop(ctx);
  return is;
}

static duk_ret_t duk__resolve_module(duk_context *ctx, void *udata) {

  // stack [... id require parent_id]

  dukext_t *vm = (dukext_t *)udata;

  const char *module_id = duk_require_string(ctx, -3);
  const char *parent_id = duk_require_string(ctx, -1);

  bool builtin = false;

  if (dukext_module_has(vm, module_id)) {
    builtin = true;
  }

  duk_idx_t obj_idx = duk_push_object(ctx);
  duk_push_array(ctx);
  duk_put_prop_string(ctx, obj_idx, "files");
  duk_dup(ctx, -4);
  duk_put_prop_string(ctx, obj_idx, "id");
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, obj_idx, "parent");

  duk_dup(ctx, -4);

  if (require_type_check(ctx, "protocol")) {
    printf("is protocol");
  } else if (require_type_check(ctx, "file")) {

    if (!cs_path_is_abs(module_id)) {
      if (strlen(parent_id) == 0) {
        parent_id = duk_get_main(ctx);
      }

      int dl = cs_path_dir(parent_id);
      char parent_path[dl + 1];
      strncpy(parent_path, parent_id, dl);
      parent_path[dl] = '\0';

      char *full_file = cs_path_join(NULL, parent_path, module_id, NULL);

      if (!full_file) {
        duk_type_error(ctx, "error");
      }

      duk_push_string(ctx, "file://");
      duk_push_string(ctx, full_file);
      free(full_file);
      duk_concat(ctx, 2);
      duk_put_prop_string(ctx, obj_idx, "id");
    }
  } else {
    duk_push_string(ctx, "module://");
    duk_dup(ctx, -2);
    duk_concat(ctx, 2);
    duk_put_prop_string(ctx, obj_idx, "id");
  }

  duk_pop(ctx);

  duk_push_c_lightfunc(ctx, match_reg, 2, 2, 0);
  duk_push_string(ctx, "protocol");
  duk_get_prop_string(ctx, obj_idx, "id");
  duk_ret_t ret = duk_pcall(ctx, 2);
  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }
  duk_put_prop_string(ctx, obj_idx, "protocol");

  duk_get_prop_string(ctx, obj_idx, "protocol");
  duk_get_prop_index(ctx, -1, 1);
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "find_resolver");

  duk_dup(ctx, -3);
  ret = duk_pcall(ctx, 1);
  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }

  if (duk_is_null_or_undefined(ctx, -1)) {
    duk_type_error(ctx, "could not find resolver for protocol: '%s'",
                   duk_require_string(ctx, -3));
  }

  duk_get_prop_string(ctx, -1, "resolve");
  duk_dup(ctx, obj_idx);
  ret = duk_pcall(ctx, 1);
  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }
  duk_pop(ctx);
  duk_put_prop_string(ctx, obj_idx, "resolver");

  duk_pop_n(ctx, 3);

  return 1;
}

void dukextp_init_commonjs(dukext_t *vm) {
  duk_context *ctx = vm->ctx;

  duk_push_global_stash(ctx);

  duk_push_bare_object(ctx);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("requireCache"));
  duk_pop(ctx);

  /* Stash main module. */
  duk_push_global_stash(ctx);
  duk_push_undefined(ctx);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("mainModule"));
  duk_pop(ctx);

  /* register `require` as a global function. */
  duk_push_global_object(ctx);
  duk_push_string(ctx, "require");
  duk__push_require_function(ctx, "");
  duk_def_prop(ctx, -3,
               DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE |
                   DUK_DEFPROP_SET_CONFIGURABLE);
  duk_pop(ctx);
}

duk_ret_t dukextp_commonjs_eval_main(duk_context *ctx, const char *path) {
  /*
   *  Stack: [ ... source ]
   */

  duk__push_module_object(ctx, path, 1 /*main*/);
  /* [ ... source module ] */

  duk_dup(ctx, 0);
  /* [ ... source module source ] */

  return duk_safe_call(ctx, duk__eval_module_source, NULL, 2, 1);
}

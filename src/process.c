#include "process.h"
#include "utils.h"
#include <csystem/standardpaths.h>

static dump_context(duk_context *ctx) {
  duk_push_context_dump(ctx);
  fprintf(stdout, "%s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);
}
static duk_ret_t process_cwd(duk_context *ctx) {
  char *buffer = cs_getcwd(NULL, 0);
  if (!buffer)
    return 0;
  duk_push_string(ctx, buffer);
  free(buffer);

  return 1;
}

static duk_ret_t process_get_env(duk_context *ctx) {

  const char *name = duk_require_string(ctx, 1);

  const char *out = getenv(name);
  if (!out)
    return 0;

  duk_push_string(ctx, out);

  return 1;
}

static duk_ret_t process_set_env(duk_context *ctx) {

  const char *name = duk_require_string(ctx, 1);
  const char *value = duk_require_string(ctx, 2);

  setenv(name, value, 1);

  return 0;
}

extern char **environ;

static duk_ret_t process_ownkeys_env(duk_context *ctx) {
  int i = 1;
  char *s = *environ;

  duk_idx_t idx = duk_push_array(ctx);

  for (; s; i++) {
    size_t ki = cs_str_indexof(s, '=');
    duk_push_string(ctx, s + ki + 1);
    duk_put_prop_index(ctx, idx, i - 1);
    s = *(environ + i);
  }

  return 1;
}

static duk_ret_t process_has_env(duk_context *ctx) {

  const char *n = getenv(duk_require_string(ctx, 1));

  duk_push_boolean(ctx, n != NULL);

  return 1;
}

static void init_env(duk_context *ctx) {
  duk_push_object(ctx); // target
  duk_push_object(ctx); // handle
  duk_push_c_function(ctx, process_get_env, 2);
  duk_put_prop_string(ctx, -2, "get");
  duk_push_c_function(ctx, process_set_env, 3);
  duk_put_prop_string(ctx, -2, "set");
  duk_push_c_function(ctx, process_ownkeys_env, 2);
  duk_put_prop_string(ctx, -2, "ownKeys");

  duk_push_c_function(ctx, process_has_env, 2);
  duk_put_prop_string(ctx, -2, "has");

  duk_push_proxy(ctx, 0);
}

void dk_module_process_init(struct duker_s *ctx) {
  duk_push_global_object(ctx->ctx);
  duk_idx_t idx = duk_push_object(ctx->ctx);

  duk_push_c_lightfunc(ctx->ctx, process_cwd, 0, 1, 0);
  duk_put_prop_string(ctx->ctx, idx, "cwd");

  init_env(ctx->ctx);
  duk_put_prop_string(ctx->ctx, idx, "env");

  duk_put_prop_string(ctx->ctx, -2, "process");
}
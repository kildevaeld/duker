#include <duker/duker.h>
#include <stdio.h>

static duk_ret_t plugin(duk_context *ctx) {

  printf("hello dig\n");
  duk_idx_t idx = duk_push_object(ctx);

  duk_push_string(ctx, "Hello, world");
  duk_put_prop_string(ctx, idx, "test");
  // dk_dump_context_stdout(ctx);
  return 1;
}

int main(int argc, const char **argv) {

  duk_context *ctx = duk_create_heap_default();
  if (!ctx) {
    printf("rapper\n");
    return 0;
  }

  duker_t *d;
  if (!(d = dk_create(ctx))) {
    printf("could not init duk\n");
    return 1;
  };

  dk_add_default_modules(d);

  // dk_add_module_fn(d, "./other.js", plugin);

  if (argc > 1) {
    const char *path = argv[1];

    duk_ret_t ret = dk_eval_path(d, path);
    if (ret != DUK_EXEC_SUCCESS) {
      if (duk_get_prop_string(ctx, -1, "stack")) {
        duk_replace(ctx, -2);
      } else {
        duk_pop(ctx);
      }
      printf("--> %s\n", duk_safe_to_string(ctx, -1));
      duk_pop(ctx);
    }
  }

  dk_free(d);
  // duk_eval_string(ctx, "console.log('Hello, World')");
  // printf("result %d\n", duk_get_int(ctx, -1));
  return 0;
}
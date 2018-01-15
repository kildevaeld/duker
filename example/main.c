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

  duker_t *d;
  if (!(d = dk_create(NULL))) {
    printf("could not init duk\n");
    return 1;
  };

  dk_add_default_modules(d);

  int ret = EXIT_SUCCESS;
  if (argc > 1) {
    const char *path = argv[1];
    duker_err_t *err = NULL;
    duk_ret_t ret = dk_eval_path(d, path, &err);

    if (ret != DUK_EXEC_SUCCESS) {
      printf("error %s\n", err->message);
      dk_free_err(err);
    }
  } else {
    fprintf(stderr, "usage: duker <path>\n");
    ret = EXIT_FAILURE;
  }

  dk_free(d);

  return ret;
}
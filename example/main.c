#include <duker/duker.h>
//#include <duker/pool.h>
#include <duker/uv/uv-module.h>
#include <stdio.h>

// Run a single file, one time
static int run_single(const char *path) {

  duker_t *d;
  if (!(d = dukext_create(NULL))) {
    printf("could not init duk\n");
    return 1;
  };

  uv_loop_t *loop = uv_default_loop();

  dukext_register_module_uv(d, loop);
  dukext_add_default_modules(d);

  duker_err_t *err = NULL;
  duk_ret_t ret = dukext_eval_path(d, path, &err);

  uv_run(loop, UV_RUN_DEFAULT);

  if (ret != DUK_EXEC_SUCCESS) {
    printf("error %s\n", err->message);
    dukext_free_err(err);
  }
  dukext_free(d);

  return ret;
}

/*
static duker_t *create_ctx() {
  duker_t *ctx = dukext_create(NULL);
  dukext_register_module_uv(ctx, uv_loop_new());
  return ctx;
}

static int run_pool(const char **path, size_t count, int n) {

  duker_pool_t *pool = dukext_create_pool(4, create_ctx, NULL);
  int i = 0;
  while (i < count) {
    dukext_pool_add_path(pool, path[i]);
    i++;
  }

  dukext_pool_wait(pool);
  dukext_free_pool(pool);

  return 0;
}*/

int main(int argc, const char **argv) {

  if (argc == 1) {
    fprintf(stderr, "usage: duker [-pN] <path>\n");
    return EXIT_FAILURE;
  } else if (argc == 2 || strncmp(argv[1], "-p", 2) != 0) {
    return run_single(argv[1]);
  } else if (strncmp(argv[1], "-p", 2) == 0) {
    // int n = 4;
    // return run_pool(&argv[2], argc - 2, n);
  }

  return 0;
}
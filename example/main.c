#include <dukext/dukext.h>
#include <dukext/module.h>
//#include <duker/pool.h>
#include <dukext/uv/uv.h>
#include <stdio.h>

static duk_ret_t mod(duk_context *ctx) {
  duk_push_string(ctx, "builtint");
  return 1;
}

// Run a single file, one time
static int run_single(const char *path) {

  dukext_config_t config;
  dukext_config_init(&config);
  config.max_heap = 1024 << 16;
  config.module_types = DUKEXT_FILE_TYPE;
  config.modules = DUKEXT_MOD_FILE | DUKEXT_MOD_PROMPT;
  dukext_t *vm;
  if (!(vm = dukext_create(config))) {
    printf("could not init duk\n");
    return 1;
  };
  dukext_dump_stats(vm);
  dukext_uv_init(vm, uv_default_loop());

  dukext_module_set(vm, "test", mod);
  dukext_err_t *err = NULL;
  duk_ret_t ret = dukext_eval_path(vm, path, &err);

  dukext_dump_stats(vm);

  // uv_run(loop, UV_RUN_DEFAULT);

  if (ret != DUK_EXEC_SUCCESS) {
    printf("error %s\n", err->message);
    dukext_err_free(err);
    goto end;
  }

  // sleep(100);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  dukext_dump_stats(vm);
end:
  dukext_destroy(vm);

  return ret;
}

/*
static dukext_t *create_ctx() {
  dukext_t *ctx = dukext_create(NULL);
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
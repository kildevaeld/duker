#include <dukext/dukext.h>
#include <dukext/module.h>
//#include <duker/pool.h>
#include <dukext/curl/curl.h>
#include <dukext/io/io.h>
#include <stdio.h>

static duk_ret_t mod(duk_context *ctx) {
  duk_push_string(ctx, "builtint");
  return 1;
}

// Run a single file, one time
static int run_single(const char *path) {

  dukext_config_t config;
  dukext_config_init(&config);
  // config.max_heap = 1024 << 24;
  config.module_types = DUKEXT_FILE_TYPE;
  config.modules = DUKEXT_MOD_FILE | DUKEXT_MOD_PROMPT;

  dukext_t *vm;
  if (!(vm = dukext_create(config))) {
    printf("could not init duk\n");
    return 1;
  };

  dukext_io_init(vm);
  dukext_curl_init(vm);

  
  dukext_module_set(vm, "test", mod);
  dukext_err_t *err = NULL;
  duk_ret_t ret = dukext_eval_path(vm, path, &err);

  if (ret != DUK_EXEC_SUCCESS) {
    printf("error %s\n", err->message);
    dukext_err_free(err);
    goto end;
  }

  // sleep(100);
end:
  dukext_destroy(vm);

  return ret;
}

int main(int argc, const char **argv) {

  if (argc == 1) {
    fprintf(stderr, "usage: dukext <path>\n");
  }

  return run_single(argv[1]);
}
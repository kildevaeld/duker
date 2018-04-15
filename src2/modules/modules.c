#include "modules.h"
#include <dukext/module.h>

extern duk_ret_t dukext_module_fs(duk_context *ctx);
extern duk_ret_t dukext_module_term(duk_context *ctx);

void dukext_init_modules(dukext_t *vm) {
  dukext_module_set(vm, "fs", dukext_module_fs);
  dukext_module_set(vm, "term", dukext_module_term);
}
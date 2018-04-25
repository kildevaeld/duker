#include "modules.h"
#include "javascript.h"
#include <dukext/module.h>

extern duk_ret_t dukext_module_fs(duk_context *ctx);
extern duk_ret_t dukext_module_prompt(duk_context *ctx);

void dukext_init_modules(dukext_t *vm) {

  dukext_config_t cfg = dukext_get_config(vm);

  dukext_module_lstring_set(vm, "util", file_format, file_format_len);

  if (cfg.modules & DUKEXT_MOD_FILE)
    dukext_module_set(vm, "fs", dukext_module_fs);
  if (cfg.modules & DUKEXT_MOD_PROMPT)
    dukext_module_set(vm, "prompt", dukext_module_prompt);
}
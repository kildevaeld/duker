#include <dukext/module.h>
#include <dukext/zip.h>

duk_ret_t zip_resolve_file(duk_context *ctx) {}
duk_ret_t zip_load_file(duk_context *ctx) {}

void dukext_register_zip_resolver(dukext_t *vm) {
  dukext_register_zip_resolver_named(vm, "zip");
}
void dukext_register_zip_resolver_named(dukext_t *vm, const char *name) {
  dukext_set_module_resolver(vm, name, zip_resolve_file, zip_load_file);
}
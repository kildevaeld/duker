#pragma once
#include <dukext/dukext.h>

void dukext_register_zip_resolver(dukext_t *vm);
void dukext_register_zip_resolver_named(dukext_t *vm, const char *name);
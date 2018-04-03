#pragma once
#include <duktape.h>

void *sandbox_alloc(void *udata, duk_size_t size);
void *sandbox_realloc(void *udata, void *ptr, duk_size_t size);
void sandbox_free(void *udata, void *ptr);
void sandbox_fatal(void *udata, const char *msg);
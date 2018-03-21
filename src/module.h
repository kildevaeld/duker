#pragma once
//#include <duker/duker.h>
#include "type.h"
#include <duktape.h>

typedef duk_ret_t (*dk_module_initializer)(duk_context *ctx);

void init_modules(duk_context *ctx);

int add_module_fn(struct duker_s *ctx, const char *name,
                  duk_ret_t (*fn)(duk_context *));
int add_module_str(struct duker_s *ctx, const char *name, const char *content);

int add_module_lib(struct duker_s *ctx, const char *n, void *handle);

struct modules_bag_s *get_module(struct duker_s *ctx, const char *name);

void free_modules(duker_t *ctx);
#pragma once
//#include <duker/duker.h>
#include "type.h"
#include <duktape.h>

void init_modules(duk_context *ctx);

int add_module_fn(struct duker_s *ctx, const char *name,
                  duk_ret_t (*fn)(duk_context *));
int add_module_str(struct duker_s *ctx, const char *name, const char *content);

struct modules_bag_s *get_module(struct duker_s *ctx, const char *name);

void free_modules(duker_t *ctx);
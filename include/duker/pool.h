#pragma once
#include <duktape.h>

typedef duk_context *(*dk_context_creator)(void *data);

typedef struct duker_pool_s duker_pool_t;

duker_pool_t *dk_create_pool_default(int number);
duker_pool_t *dk_create_pool(int number, dk_context_creator fn, void *udata);
void dk_free_pool(duker_pool_t *pool);

void dk_pool_add_path(duker_pool_t *pool, const char *path);
void dk_pool_add_script(duker_pool_t *pool, const char *script);
void dk_pool_wait(duker_pool_t *pool);
void dk_pool_start(duker_pool_t *pool);
#include "type.h"
#include "uv/uv-module.h"
#include <duker/pool.h>
#include <pthread.h>
#include <thpool.h>

struct duker_pool_s {
  duker_t **ctxs;
  int c_idle;
  int c_inuse;
  pthread_mutex_t mutex;
  threadpool thpool;
};

struct duker_pool_task {
  duker_pool_t *pool;
  char *script;
};

static duker_t *take_ctx(duker_pool_t *pool) {
  duker_t *ctx = NULL;
  pthread_mutex_lock(&pool->mutex);
  if (pool->c_idle > 0) {
    ctx = pool->ctxs[pool->c_inuse++];
    pool->c_idle--;
  }
  pthread_mutex_unlock(&pool->mutex);
  return ctx;
}

static void put_ctx(duker_pool_t *pool, duker_t *ctx) {
  pthread_mutex_lock(&pool->mutex);
  if (pool->c_inuse > 0) {
    ctx = pool->ctxs[pool->c_inuse--];
    pool->c_idle++;
  }
  pthread_mutex_unlock(&pool->mutex);
}

static void worker_thread(void *data) {

  struct duker_pool_task *task = (struct duker_pool_task *)data;

  duker_t *ctx = take_ctx(task->pool);

  duk_push_global_object(ctx->ctx);
  duk_push_number(ctx->ctx, (int)pthread_self());
  duk_put_prop_string(ctx->ctx, -2, "thread_id");
  duk_pop(ctx->ctx);

  duker_err_t *err = NULL;
  if (dk_eval_path(ctx, task->script, &err) != DUK_EXEC_SUCCESS) {
    fprintf(stderr, "error: path %s: %s", task->script, err->message);
    dk_free_err(err);
  }

  uv_loop_t *loop = dk_stash_get_ptr(ctx->ctx, "uv_loop");

  uv_run(loop, UV_RUN_DEFAULT);

  // Cleaning
  put_ctx(task->pool, ctx);

  free(task->script);
  free(task);
}

static duk_context *create_context(void *d) {
  return duk_create_heap_default();
}

duker_pool_t *dk_create_pool_default(int number) {
  duk_context *ctx = dk_create_pool(number, create_context, NULL);
  dk_add_default_modules(ctx);
  return ctx;
}

duker_pool_t *dk_create_pool(int number, dk_context_creator fn, void *udata) {
  duker_pool_t *pool = malloc(sizeof(duker_pool_t));
  pool->ctxs = malloc(sizeof(duker_t) * number);
  pool->c_idle = number;
  pool->c_inuse = 0;

  int n = number;

  while (n--) {
    pool->ctxs[n] = fn(udata);
  }
  pool->thpool = thpool_init(number);

  return pool;
}

void dk_pool_wait(duker_pool_t *pool) { thpool_wait(pool->thpool); }

void dk_pool_start(duker_pool_t *pool) { thpool_resume(pool->thpool); }

void dk_free_pool(duker_pool_t *p) {
  thpool_wait(p->thpool);
  thpool_destroy(p->thpool);
  while (p->c_idle--) {
    dk_free(p->ctxs[p->c_idle]);
  }
  free(p->ctxs);
  free(p);
}

void dk_pool_add_path(duker_pool_t *pool, const char *path) {
  struct duker_pool_task *task = malloc(sizeof(struct duker_pool_task));
  task->pool = pool;
  task->script = strdup(path);

  thpool_add_work(pool->thpool, (void *)worker_thread, task);
}

void dk_pool_add_script(duker_pool_t *pool, const char *script) {}

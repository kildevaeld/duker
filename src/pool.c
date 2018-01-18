#include "type.h"
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
  const char *script;
  
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

duker_pool_t *dk_create_pool(int number) {
  duker_pool_t *pool = malloc(sizeof(duker_pool_t));
  pool->ctxs = malloc(sizeof(duker_t) * number);
  pool->c_idle = number;
  pool->c_inuse = 0;

  while (--number >= 0) {
    pool->ctxs[number] = dk_create(NULL);
  }

  pool->thpool = thpool_init(number);

  return pool;
}
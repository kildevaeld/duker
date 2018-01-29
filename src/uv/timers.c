#include "timers.h"
#include <duker/refs.h>
#include <stdbool.h>
#include <uv.h>
struct bag_ {
  int ref;
  duk_context *ctx;
  int repeat;
};

static void callback(uv_timer_t *t) {
  struct bag_ *b = (struct bag_ *)t->data;

  if (!b->repeat)
    uv_timer_stop(t);
  dk_push_ref(b->ctx, b->ref);
  if (!duk_is_function(b->ctx, -1))
    printf("not a function!");
  duk_call(b->ctx, 0);

  if (!b->repeat)
    free(b);
}

static int push_timer(duk_context *ctx, uv_loop_t *loop, int ref, int timeout,
                      bool repeated) {

  uv_timer_t *timer_req = duk_push_fixed_buffer(ctx, sizeof(uv_timer_t));
  uv_timer_init(loop, timer_req);

  struct bag_ *b = malloc(sizeof(struct bag_));
  b->ref = ref;
  b->ctx = ctx;
  b->repeat = repeated ? timeout : 0;

  timer_req->data = b;

  uv_timer_start(timer_req, callback, timeout, timeout);

  return 1;
}

static duk_ret_t create_timeout(duk_context *ctx) {
  uv_loop_t *loop = dk_stash_get_ptr(ctx, "uv_loop");

  if (!duk_is_function(ctx, 0)) {
    duk_type_error(ctx, "first params must be a function");
  }

  int timeout = 0;
  if (duk_is_number(ctx, 1)) {
    timeout = duk_require_int(ctx, 1);
  }

  duk_dup(ctx, 0);
  int ref = dk_ref(ctx);
  return push_timer(ctx, loop, ref, timeout, false);
}

static duk_ret_t create_interval(duk_context *ctx) {
  uv_loop_t *loop = dk_stash_get_ptr(ctx, "uv_loop");

  if (!duk_is_function(ctx, 0)) {
    duk_type_error(ctx, "first params must be a function");
  }

  duk_dup(ctx, 0);
  int ref = dk_ref(ctx);

  int timeout = 0;
  if (duk_is_number(ctx, 1)) {
    timeout = duk_require_int(ctx, 1);
  }

  return push_timer(ctx, loop, ref, timeout, true);
}

int init_timers(duker_t *ctx, uv_loop_t *loop) {
  duk_push_global_object(ctx->ctx);

  duk_push_c_function(ctx->ctx, create_timeout, 2);
  duk_put_prop_string(ctx->ctx, -2, "setTimeout");
  duk_push_c_function(ctx->ctx, create_interval, 2);
  duk_put_prop_string(ctx->ctx, -2, "setInterval");

  duk_pop(ctx->ctx);

  return 0;
}
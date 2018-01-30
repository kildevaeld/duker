#include "timers.h"
#include <duker/refs.h>
#include <stdbool.h>
#include <uv.h>

struct tbag_ {
  int ref;
  int tref;
  duk_context *ctx;
  int repeat;
};

static void callback(uv_timer_t *t) {
  struct tbag_ *b = (struct tbag_ *)t->data;

  duk_idx_t top = duk_get_top_index(b->ctx);
  dk_push_ref(b->ctx, b->ref);

  duk_call(b->ctx, 0);
  if (duk_get_top_index(b->ctx) > top)
    duk_pop(b->ctx);

  if (!b->repeat) {
    uv_timer_stop(t);
    dk_unref(b->ctx, b->ref);
    dk_unref(b->ctx, b->tref);
    free(b);
  }
}

static int push_timer(duk_context *ctx, uv_loop_t *loop, int ref, int timeout,
                      bool repeated) {

  uv_timer_t *timer_req = duk_push_fixed_buffer(ctx, sizeof(uv_timer_t));
  uv_timer_init(loop, timer_req);

  int tref = dk_ref(ctx);
  duk_pop(ctx);
  duk_push_int(ctx, tref);

  struct tbag_ *b = malloc(sizeof(struct tbag_));
  b->ref = ref;
  b->tref = tref;
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

  int magic = duk_get_current_magic(ctx);

  duk_dup(ctx, 0);
  int ref = dk_ref(ctx);
  return push_timer(ctx, loop, ref, timeout, magic == 1);
}

static duk_ret_t clear_timeout(duk_context *ctx) {

  int ref = duk_require_int(ctx, 0);
  dk_push_ref(ctx, ref);

  duk_size_t size;
  uv_timer_t *timer = duk_require_buffer(ctx, -1, &size);

  if (!timer || size != sizeof(uv_timer_t)) {
    return 0;
  }

  struct tbag_ *b = (struct tbag_ *)timer->data;
  dk_unref(ctx, b->ref);
  dk_unref(ctx, b->tref);
  free(b);

  uv_timer_stop(timer);

  return 0;
}

int init_timers(duker_t *ctx, uv_loop_t *loop) {
  duk_push_global_object(ctx->ctx);

  duk_push_c_function(ctx->ctx, create_timeout, 2);
  duk_set_magic(ctx->ctx, -1, 0);
  duk_put_prop_string(ctx->ctx, -2, "setTimeout");

  duk_push_c_function(ctx->ctx, create_timeout, 2);
  duk_set_magic(ctx->ctx, -1, 1);
  duk_put_prop_string(ctx->ctx, -2, "setInterval");

  duk_push_c_function(ctx->ctx, clear_timeout, 1);
  duk_put_prop_string(ctx->ctx, -2, "clearTimeout");

  duk_push_c_function(ctx->ctx, clear_timeout, 1);
  duk_put_prop_string(ctx->ctx, -2, "clearInterval");

  duk_pop(ctx->ctx);

  return 0;
}
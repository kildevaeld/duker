#include "timers.h"
#include "bag.h"
#include <dukext/utils.h>

static void callback(uv_timer_t *t) {
  uv_bag_t *b = (uv_bag_t *)t->data;

  duk_context *ctx = b->ctx;

  duk_push_ref(ctx, b->ref);

  duk_idx_t idx = duk_normalize_index(ctx, -1);

  duk_get_prop_string(ctx, idx, "cb");
  duk_call(ctx, 0);
  duk_pop(ctx);

  duk_get_prop_string(ctx, idx, "repeat");
  if (!duk_to_boolean(ctx, -1)) {
    uv_timer_stop(t);
    dukext_uv_bag_put(b);
  }
  duk_pop_2(ctx);
}

static duk_ret_t create_timeout(duk_context *ctx) {
  uv_loop_t *loop = duk_stash_get_ptr(ctx, "dukext_uv");

  if (!duk_is_function(ctx, 0)) {
    duk_type_error(ctx, "first params must be a function");
  }

  int timeout = 0;
  if (duk_is_number(ctx, 1)) {
    timeout = duk_require_int(ctx, 1);
  }

  int magic = duk_get_current_magic(ctx);

  duk_idx_t oidx = duk_push_object(ctx);

  uv_timer_t *timer_req = duk_push_fixed_buffer(ctx, sizeof(uv_timer_t));
  uv_timer_init(loop, timer_req);
  duk_put_prop_string(ctx, oidx, "timer");

  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, oidx, "cb");

  duk_push_int(ctx, timeout);
  duk_put_prop_string(ctx, oidx, "timeout");

  duk_push_boolean(ctx, magic);
  duk_put_prop_string(ctx, oidx, "repeat");

  int ref = duk_ref(ctx);

  uv_bag_t *bag = dukext_uv_bag_get(ctx, ref);

  timer_req->data = bag;

  uv_timer_start(timer_req, callback, timeout, magic ? timeout : 0);

  duk_push_int(ctx, ref);

  return 1;
}

static duk_ret_t clear_timeout(duk_context *ctx) {

  int ref = duk_require_int(ctx, 0);
  duk_push_ref(ctx, ref);

  if (!duk_is_object(ctx, -1)) {
    return 0;
  }

  if (!duk_has_prop_string(ctx, -1, "timer")) {
    return 0;
  }

  duk_get_prop_string(ctx, -1, "timer");

  duk_size_t size;
  uv_timer_t *timer = duk_require_buffer(ctx, -1, &size);

  if (!timer || size != sizeof(uv_timer_t)) {
    return 0;
  }

  uv_bag_t *b = (uv_bag_t *)timer->data;
  uv_timer_stop(timer);

  dukext_uv_bag_put(b);

  return 0;
}

void dukext_uv_timers_push(duk_context *ctx) {
  duk_push_global_object(ctx);

  duk_push_c_function(ctx, create_timeout, 2);
  duk_set_magic(ctx, -1, 0);
  duk_put_prop_string(ctx, -2, "setTimeout");

  duk_push_c_function(ctx, create_timeout, 2);
  duk_set_magic(ctx, -1, 1);
  duk_put_prop_string(ctx, -2, "setInterval");

  duk_push_c_function(ctx, clear_timeout, 1);
  duk_put_prop_string(ctx, -2, "clearTimeout");

  duk_push_c_function(ctx, clear_timeout, 1);
  duk_put_prop_string(ctx, -2, "clearInterval");

  duk_pop(ctx);
}
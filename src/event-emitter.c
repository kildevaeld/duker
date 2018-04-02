#include "event-emitter.h"

static duk_ret_t event_emitter_constructor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  // Get access to the default instance.
  duk_push_this(ctx); // -> stack: [ name this ]

  duk_push_object(ctx);

  duk_put_prop_string(ctx, -2, "_listeners");

  // Set this.name to name.
  // duk_dup(ctx, 0);                      // -> stack: [ name this name ]
  // duk_put_prop_string(ctx, -2, "name"); // -> stack: [ name this ]

  /* Return undefined: default instance will be used. */
  return 0;
}

static duk_ret_t event_emitter_on(duk_context *ctx) {

  const char *eventName = duk_require_string(ctx, 0);
  duk_require_function(ctx, 1);
  duk_idx_t idx = duk_normalize_index(ctx, 1);

  duk_idx_t ctx_id = -1;

  if (!duk_is_null_or_undefined(ctx, 2)) {
    ctx_id = duk_normalize_index(ctx, 2);
  }

  duk_push_this(ctx);

  duk_get_prop_string(ctx, -1, "_listeners");

  if (!duk_get_prop_string(ctx, -1, eventName)) {
    duk_pop(ctx);
    duk_push_array(ctx);

    duk_put_prop_string(ctx, -2, eventName);
    duk_get_prop_string(ctx, -1, eventName);
  }

  // duk_get_prop_string(ctx, -1, eventName);

  duk_size_t len = duk_get_length(ctx, -1);

  duk_push_object(ctx);
  duk_dup(ctx, idx);

  duk_put_prop_string(ctx, -2, "cb");

  duk_push_boolean(ctx, duk_get_current_magic(ctx));
  duk_put_prop_string(ctx, -2, "once");

  if (ctx_id != -1) {
    duk_dup(ctx, ctx_id);
  } else {
    duk_push_undefined(ctx);
  }
  duk_put_prop_string(ctx, -2, "ctx");

  duk_put_prop_index(ctx, -2, len);

  duk_pop_2(ctx);

  return 1;
}

static duk_ret_t event_emitter_off(duk_context *ctx) {

  duk_size_t len = duk_get_top(ctx);

  const char *event = NULL;

  duk_push_this(ctx);

  return 1;
}

static duk_ret_t event_emitter_emit(duk_context *ctx) {

  const char *eventName = duk_require_string(ctx, 0);
  duk_size_t len = duk_get_top(ctx);

  duk_push_array(ctx);

  for (int i = 1; i < len; i++) {
    duk_dup(ctx, i);
    duk_put_prop_index(ctx, -2, i - 1);
  }

  duk_idx_t idx = duk_normalize_index(ctx, -1);

  duk_push_this(ctx);

  duk_get_prop_string(ctx, -1, "_listeners");
  duk_get_prop_string(ctx, -1, eventName);

  if (duk_is_undefined(ctx, -1)) {
    dukext_dump_context_stdout(ctx);
    return 1;
  }

  duk_size_t llen = duk_get_length(ctx, -1);

  for (int i = 0; i < llen; i++) {

    duk_get_prop_index(ctx, -1, i);
    duk_idx_t oidx = duk_normalize_index(ctx, -1);

    duk_get_prop_string(ctx, oidx, "cb");
    duk_get_prop_string(ctx, oidx, "ctx");

    for (int i = 0; i < len; i++) {
      duk_get_prop_index(ctx, idx, i);
    }

    int rc = duk_pcall_method(ctx, len);
    /*if (rc == DUK_EXEC_SUCCESS) {
      printf("2+3=%ld\n", (long)duk_get_int(ctx, -1));
    } else {
      printf("error: %s\n", duk_to_string(ctx, -1));
    }*/

    duk_pop_2(ctx);
  }
  duk_pop_2(ctx);

  return 1;
}

static void push_event_emitter_type(duk_context *ctx) {
  /* Push constructor function; all Duktape/C functions are
   * "constructable" and can be called as 'new Foo()'.
   */
  duk_push_c_function(ctx, event_emitter_constructor, 0 /*nargs*/);

  /* Push MyObject.prototype object. */
  duk_push_object(ctx); /* -> stack: [ MyObject proto ] */

  duk_push_c_function(ctx, event_emitter_on, 3);
  duk_set_magic(ctx, -1, 0);
  duk_put_prop_string(ctx, -2, "on");

  duk_push_c_function(ctx, event_emitter_on, 2);
  duk_set_magic(ctx, -1, 1);
  duk_put_prop_string(ctx, -2, "once");

  duk_push_c_function(ctx, event_emitter_off, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "off");

  duk_push_c_function(ctx, event_emitter_emit, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "emit");

  duk_put_prop_string(ctx, -2, "prototype");
}

static duk_ret_t push_event_emitter(duk_context *ctx) {
  duk_push_object(ctx);
  push_event_emitter_type(ctx);
  duk_put_prop_string(ctx, -2, "EventEmitter");
  return 1;
}

int dukext_register_module_event_emitter(struct duker_s *ctx) {
  dukext_add_module_fn(ctx, "events", push_event_emitter);
  return 1;
}
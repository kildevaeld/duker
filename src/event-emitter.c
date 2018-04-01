#include "event-emitter.h"

static duk_ret_t event_emitter_constructor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  // Get access to the default instance.
  duk_push_this(ctx); // -> stack: [ name this ]

  duk_push_array(ctx);

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

  duk_push_this(ctx);

  return 1;
}

static duk_ret_t event_emitter_off(duk_context *ctx) {

  duk_push_this(ctx);

  return 1;
}

static duk_ret_t event_emitter_emit(duk_context *ctx) {

  duk_push_this(ctx);

  return 1;
}

static void push_event_emitter_type(duk_context *ctx) {
  /* Push constructor function; all Duktape/C functions are
   * "constructable" and can be called as 'new Foo()'.
   */
  duk_push_c_function(ctx, event_emitter_constructor, 0 /*nargs*/);

  /* Push MyObject.prototype object. */
  duk_push_object(ctx); /* -> stack: [ MyObject proto ] */

  duk_push_c_function(ctx, event_emitter_on, 2);
  duk_put_prop_string(ctx, -2, "on");

  duk_push_c_function(ctx, event_emitter_on, 2);
  duk_put_prop_string(ctx, -2, "once");

  duk_push_c_function(ctx, event_emitter_off, 2);
  duk_put_prop_string(ctx, -2, "off");

  duk_push_c_function(ctx, event_emitter_emit, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "emit");

  /* Set MyObject.prototype.printName. */
  // duk_push_c_function(ctx, myobject_print_name, 0 /*nargs*/);
  // duk_put_prop_string(ctx, -2, "printName");

  /* Set MyObject.prototype = proto */
  duk_put_prop_string(ctx, -2, "prototype"); /* -> stack: [ MyObject ] */

  /* Finally, register MyObject to the global object */
  // duk_put_global_string(ctx, "MyObject"); /* -> stack: [ ] */
}

static duk_ret_t push_event_emitter(duk_context *ctx) {
  duk_push_object(ctx);
  push_event_emitter_type(ctx);
  duk_put_prop_string(ctx, -2, "EventEmitter");
  return 1;
}

int dk_register_module_event_emitter(struct duker_s *ctx) {

  dk_add_module_fn(ctx, "events", push_event_emitter);
  return 1;
}
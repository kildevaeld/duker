#include <csystem/terminal/form.h>
#include <duktape.h>

duk_ret_t de_prompt_list(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  if (!duk_is_array(ctx, 1)) {
    duk_type_error(ctx, "should be array");
  }

  int len = duk_get_length(ctx, 1);

  duk_enum(ctx, 1, 0);
  char *buf[len];
  while (duk_next(ctx, -1, 1)) {
    buf[duk_to_int(ctx, -2)] = (char *)duk_require_string(ctx, -1);
    duk_pop_2(ctx);
  }
  duk_pop(ctx);

  int i = cs_term_form_list(msg, (const char **)buf, len);

  if (i == -1) {
    duk_push_undefined(ctx);
  } else {
    duk_get_prop_index(ctx, 1, i);
  }

  return 1;
}

duk_ret_t de_prompt_confirm(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  bool clear = duk_get_boolean_default(ctx, 1, false);

  bool ret = cs_term_form_confirm(msg, clear);
  duk_push_boolean(ctx, ret);

  return 1;
}

duk_ret_t de_prompt_input(duk_context *ctx) {
  const char *msg = duk_require_string(ctx, 0);

  char *ret = cs_term_form_input(msg);
  duk_push_string(ctx, ret);
  free(ret);

  return 1;
}

duk_ret_t dukext_module_prompt(duk_context *ctx) {

  duk_push_object(ctx);

  duk_push_c_function(ctx, de_prompt_list, 2);
  duk_put_prop_string(ctx, -2, "list");

  duk_push_c_function(ctx, de_prompt_confirm, 2);
  duk_put_prop_string(ctx, -2, "confirm");

  duk_push_c_function(ctx, de_prompt_input, 2);
  duk_put_prop_string(ctx, -2, "input");

  return 1;
}
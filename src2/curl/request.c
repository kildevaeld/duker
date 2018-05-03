#include <dukext/utils.h>
#include <duktape.h>
#include <stdbool.h>

static duk_ret_t curl_request_dtor(duk_context *ctx) { return 0; }

static bool validate_options(duk_context *ctx, char **err) {

  if (duk_has_prop_string(ctx, 0, "header")) {
    duk_get_prop_string(ctx, 0, "header");
    if (!duk_is_object(ctx, -1)) {
      duk_pop(ctx);
      *err = "header should be an object";
      return false;
    }
    duk_pop(ctx);
  }

  if (duk_has_prop_string(ctx, 0, "url")) {
    duk_get_prop_string(ctx, 0, "url");
    if (!duk_is_string(ctx, -1)) {
      duk_pop(ctx);
      *err = "url should be a string";
      return false;
    }
    duk_pop(ctx);
  }

  if (duk_has_prop_string(ctx, 0, "method")) {
    duk_get_prop_string(ctx, 0, "method");
    if (!duk_is_string(ctx, -1)) {
      duk_pop(ctx);
      *err = "method should be a string";
      return false;
    }
    duk_pop(ctx);
  }

  if (duk_has_prop_string(ctx, 0, "progress")) {
    duk_get_prop_string(ctx, 0, "url");
    if (!duk_is_function(ctx, -1)) {
      duk_pop(ctx);
      *err = "progress should be a function";
      return false;
    }
    duk_pop(ctx);
  }

  char *fields[] = {"progress", "method", "url", "header"};
  size_t len = sizeof(fields);
  duk_push_object(ctx);
  for (int i = 0; i < 4; i++) {
    if (duk_has_prop_string(ctx, 0, fields[i])) {
      duk_get_prop_string(ctx, 0, fields[i]);
      duk_put_prop_string(ctx, -2, fields[i]);
    }
  }

  duk_replace(ctx, 0);

  return true;
}

static duk_ret_t curl_request_ctor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }
  duk_push_this(ctx);
  if (duk_is_object(ctx, 0)) {
    char *err = NULL;
    if (!validate_options(ctx, &err)) {
      duk_type_error(ctx, "invalid options: ", err);
    }

    duk_dup(ctx, 0);

  } else {
    duk_push_object(ctx);
  }
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("_options"));

  return 0;
}

static duk_ret_t curl_request_set(duk_context *ctx) {

  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_get_prop_string(ctx, -1, "header");
  if (duk_is_null_or_undefined(ctx, -1)) {
    duk_pop(ctx);
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "header");
  }

  duk_idx_t idx = duk_normalize_index(ctx, -1);

  if (duk_is_string(ctx, 0)) {
    const char *k = duk_require_string(ctx, 0);
    duk_dup(ctx, 1);
    duk_put_prop_string(ctx, -2, k);
  } else if (duk_is_object(ctx, 0)) {
    duk_enum(ctx, 0, DUK_ENUM_OWN_PROPERTIES_ONLY);
    while (duk_next(ctx, -1, 1)) {
      const char *k = duk_require_string(ctx, -2);
      duk_put_prop_string(ctx, idx, k);
      duk_pop(ctx);
    }
    duk_pop(ctx);
  }

  duk_pop_2(ctx);
}

static duk_ret_t curl_request_get(duk_context *ctx) {
  const char *k = duk_require_string(ctx, 0);
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_get_prop_string(ctx, -1, "header");
  if (duk_is_null_or_undefined(ctx, -1)) {
    return 0;
  }

  duk_get_prop_string(ctx, -1, k);
  return 1;
}

static duk_ret_t curl_request_set_progress(duk_context *ctx) {
  duk_push_this(ctx);

  duk_require_function(ctx, 0);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));

  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "progress");

  duk_pop(ctx);
  return 0;
}

static duk_ret_t curl_request_get_progress(duk_context *ctx) {
  duk_push_this(ctx);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_get_prop_string(ctx, -1, "progress");

  return 1;
}

static duk_ret_t curl_request_set_method(duk_context *ctx) {
  duk_push_this(ctx);

  duk_require_string(ctx, 0);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));

  duk_dup(ctx, 0);
  duk_get_prop_string(ctx, -1, "toUpperCase");
  duk_call_method(ctx, 0);
  duk_put_prop_string(ctx, -2, "method");

  duk_pop(ctx);
  return 1;
}

static duk_ret_t curl_request_get_method(duk_context *ctx) {
  duk_push_this(ctx);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_get_prop_string(ctx, -1, "method");

  return 1;
}

static duk_ret_t curl_request_set_url(duk_context *ctx) {
  duk_push_this(ctx);

  duk_require_string(ctx, 0);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "url");

  duk_pop(ctx);
  return 0;
}

static duk_ret_t curl_request_get_url(duk_context *ctx) {
  duk_push_this(ctx);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_get_prop_string(ctx, -1, "url");

  return 1;
}

static duk_ret_t curl_request_set_data(duk_context *ctx) {
  duk_push_this(ctx);

  if (!duk_is_string(ctx, 0) & !duk_is_buffer(ctx, 0)) {
    duk_type_error(ctx, "string or buffer");
  }

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "data");

  duk_pop(ctx);
  return 0;
}

static duk_ret_t curl_request_get_data(duk_context *ctx) {
  duk_push_this(ctx);

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_options"));
  duk_get_prop_string(ctx, -1, "data");

  return 1;
}

void dukext_curl_push_request(duk_context *ctx) {
  duk_push_c_function(ctx, curl_request_ctor, 1);

  // duk_push_string(ctx, "Request");
  // duk_put_prop_string(ctx, -2, "name");

  duk_idx_t idx = duk_push_object(ctx); // prototype

  duk_push_c_function(ctx, curl_request_set, 2);
  duk_put_prop_string(ctx, -2, "set");

  duk_push_c_function(ctx, curl_request_get, 1);
  duk_put_prop_string(ctx, -2, "get");

  duk_push_string(ctx, "progress");
  duk_push_c_function(ctx, curl_request_get_progress, 0);
  duk_push_c_function(ctx, curl_request_set_progress, 1);
  duk_def_prop(ctx, idx,
               DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER |
                   DUK_DEFPROP_HAVE_CONFIGURABLE | /* clear */
                   DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);

  duk_push_string(ctx, "method");
  duk_push_c_function(ctx, curl_request_get_method, 0);
  duk_push_c_function(ctx, curl_request_set_method, 1);
  duk_def_prop(ctx, idx,
               DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER |
                   DUK_DEFPROP_HAVE_CONFIGURABLE | /* clear */
                   DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);

  duk_push_string(ctx, "url");
  duk_push_c_function(ctx, curl_request_get_url, 0);
  duk_push_c_function(ctx, curl_request_set_url, 1);
  duk_def_prop(ctx, idx,
               DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER |
                   DUK_DEFPROP_HAVE_CONFIGURABLE | /* clear */
                   DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);

  duk_push_string(ctx, "data");
  duk_push_c_function(ctx, curl_request_get_data, 0);
  duk_push_c_function(ctx, curl_request_set_data, 1);
  duk_def_prop(ctx, idx,
               DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER |
                   DUK_DEFPROP_HAVE_CONFIGURABLE | /* clear */
                   DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);

  duk_put_prop_string(ctx, -2, "prototype");
}
#include <curl/curl.h>
#include <duktape.h>

static duk_ret_t curl_header_dtor(duk_context *ctx) {}

static duk_ret_t curl_header_ctor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  duk_push_this(ctx);
  duk_push_array(ctx);
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("_header"));
  // Set finalizer
  duk_push_c_function(ctx, curl_header_dtor, 1);
  duk_set_finalizer(ctx, -2);

  return 0;
}

static duk_ret_t curl_header_set(duk_context *ctx) {

  duk_push_this(ctx);
  /*struct curl_slist *list = NULL;
  if (duk_has_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("slist"))) {
    list = (struct curl_slist *)duk_get_pointer(ctx, -1);
  }*/

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("_header"));

  if (duk_is_string(ctx, 0)) {
    // const char *k = duk_require_string(ctx, 0);
    // const char *v = duk_get_string(ctx, 1);
    duk_push_string(ctx, ": ");
    duk_dup(ctx, 0);
    duk_dup(ctx, 1);
    duk_join(ctx, 2);

    // list = curl_slist_append(list, duk_require_string(ctx, -1));
    duk_pop_2(ctx);
  } else if (duk_is_object(ctx, 0)) {
    duk_enum(ctx, 0, DUK_ENUM_OWN_PROPERTIES_ONLY);
    duk_push_string(ctx, ": ");
    while (duk_next(ctx, -2, 1)) {
      duk_join(ctx, 2);
      // list = curl_slist_append(list, duk_require_string(ctx, -1));
      duk_pop(ctx);
    }
    duk_pop_2(ctx);
  }

  return 1;
}

static duk_ret_t curl_header_get(duk_context *ctx) {

  duk_push_this(ctx);

  if (duk_has_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("slist"))) {
    return 0;
  }

  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("slist"));

  curl_slist *list = duk_get_pointer(ctx, -1);
  duk_pop_2(ctx);

  return 1;
}

static duk_ret_t curl_header_unset(duk_context *ctx) {

  duk_push_this(ctx);

  return 1;
}

void dukext_curl_push_header(duk_context *ctx) {

  duk_push_c_function(ctx, curl_header_ctor, 1);

  duk_push_object(ctx); // prototype

  duk_push_c_function(ctx, curl_header_set, 2);
  duk_put_prop_string(ctx, -2, "set");

  duk_push_c_function(ctx, curl_header_get, 1);
  duk_put_prop_string(ctx, -2, "get");

  duk_push_c_function(ctx, curl_header_unset, 1);
  duk_put_prop_string(ctx, -2, "unset");

  duk_put_prop_string(ctx, -2, "prototype");
}
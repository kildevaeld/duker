#pragma once
#include <duker/duker.h>
#include <duktape.h>
#include <uthash.h>

enum module_type { FN_MODTYPE, STR_MODTYPE, LIB_MODTYPE };

struct modules_bag_s {
  char *name;
  enum module_type type;
  union {
    char *script;
    duk_ret_t (*func)(duk_context *);
    void *handle;
  } module;
  UT_hash_handle hh;
};

struct duker_s {
  duk_context *ctx;
  struct modules_bag_s *modules;
  int _c; // we created duk_context;
  int _m; // use modules
};
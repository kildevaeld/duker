#pragma once
#include <duker/duker.h>
#include <duktape.h>
#include <uthash.h>

typedef enum dukext_module_type {
  DUKEXT_FN_TYPE = 1 << 0,
  DUKEXT_STR_TYPE = 1 << 1,
  DUKEXT_LIB_TYPE = 1 << 2,
  DUKEXT_PATH_TYPE = 1 << 3
} dukext_module_type;

struct modules_bag_s {
  char *name;
  enum dukext_module_type type;
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
  duker_config_t config;
  int _c; // we created duk_context;
  int _m; // use modules
};
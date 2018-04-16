#pragma once
#include <dukext/dukext.h>
#include <duktape.h>
#include <uthash.h>

struct modules_bag_s {
  char *name;
  dukext_module_type type;
  union {
    char *script;
    duk_ret_t (*func)(duk_context *);
    void *handle;
  } module;
  UT_hash_handle hh;
};

struct dukext_s {
  duk_context *ctx;
  // struct modules_bag_s *modules;
  dukext_config_t config;
  dukext_stat_t stats;
};
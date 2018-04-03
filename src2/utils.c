#include <dukext/utils.h>

void duk_stash_set_ptr(duk_context *ctx, const char *name, void *ptr) {
  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop(ctx);
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "globals");
  }

  duk_push_pointer(ctx, ptr);
  duk_put_prop_string(ctx, -2, name);
  duk_pop_2(ctx);
}
void *duk_stash_get_ptr(duk_context *ctx, const char *name) {

  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop_2(ctx);
    return NULL;
  }

  duk_get_prop_string(ctx, -1, name);
  if (duk_is_null_or_undefined(ctx, -1)) {
    return NULL;
  }
  void *c = duk_to_pointer(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

void duk_stash_rm_ptr(duk_context *ctx, const char *name) {
  duk_push_global_stash(ctx);
  if (!duk_get_prop_string(ctx, -1, "globals")) {
    duk_pop_2(ctx);
    return;
  }

  duk_del_prop_string(ctx, -1, name);
  duk_pop_2(ctx);
}

// Create a global array refs in the heap stash.
void duk_ref_setup(duk_context *ctx) {
  duk_push_heap_stash(ctx);

  // Create a new array with one `0` at index `0`.
  duk_push_array(ctx);
  duk_push_int(ctx, 0);
  duk_put_prop_index(ctx, -2, 0);
  // Store it as "refs" in the heap stash
  duk_put_prop_string(ctx, -2, "refs");

  duk_pop(ctx);
}

// like luaL_ref, but assumes storage in "refs" property of heap stash
int duk_ref(duk_context *ctx) {
  int ref;
  if (duk_is_undefined(ctx, -1)) {
    duk_pop(ctx);
    return 0;
  }
  // Get the "refs" array in the heap stash
  duk_push_heap_stash(ctx);
  duk_get_prop_string(ctx, -1, "refs");
  duk_remove(ctx, -2);

  // ref = refs[0]
  duk_get_prop_index(ctx, -1, 0);
  ref = duk_get_int(ctx, -1);
  duk_pop(ctx);

  // If there was a free slot, remove it from the list
  if (ref != 0) {
    // refs[0] = refs[ref]
    duk_get_prop_index(ctx, -1, ref);
    duk_put_prop_index(ctx, -2, 0);
  }
  // Otherwise use the end of the list
  else {
    // ref = refs.length;
    ref = duk_get_length(ctx, -1);
  }

  // swap the array and the user value in the stack
  duk_insert(ctx, -2);

  // refs[ref] = value
  duk_put_prop_index(ctx, -2, ref);

  // Remove the refs array from the stack.
  duk_pop(ctx);

  return ref;
}

void duk_push_ref(duk_context *ctx, int ref) {
  if (!ref) {
    duk_push_undefined(ctx);
    return;
  }
  // Get the "refs" array in the heap stash
  duk_push_heap_stash(ctx);
  duk_get_prop_string(ctx, -1, "refs");
  duk_remove(ctx, -2);

  duk_get_prop_index(ctx, -1, ref);

  duk_remove(ctx, -2);
}

void duk_unref(duk_context *ctx, int ref) {

  if (!ref)
    return;

  // Get the "refs" array in the heap stash
  duk_push_heap_stash(ctx);
  duk_get_prop_string(ctx, -1, "refs");
  duk_remove(ctx, -2);

  // Insert a new link in the freelist

  // refs[ref] = refs[0]
  duk_get_prop_index(ctx, -1, 0);
  duk_put_prop_index(ctx, -2, ref);
  // refs[0] = ref
  duk_push_int(ctx, ref);
  duk_put_prop_index(ctx, -2, 0);

  duk_pop(ctx);
}

const char *duk_get_main(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1,
                      "\xff"
                      "mainModule");

  duk_get_prop_string(ctx, -1, "filename");
  const char *c = duk_get_string(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

dukext_t *duk_get_dukext(duk_context *ctx) {
  return duk_stash_get_ptr(ctx, "dukext_vm");
}

void dukext_dump_context_stdout(duk_context *ctx) {
  duk_push_context_dump(ctx);
  printf("%s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);
}
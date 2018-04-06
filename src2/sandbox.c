#include "sandbox.h"
#include "definitions.h"
#include <string.h>
/*
 *  Memory allocator which backs to standard library memory functions but
 *  keeps a small header to track current allocation size.
 *
 *  Many other sandbox allocation models are useful, e.g. preallocated pools.
 */

typedef struct {
  /* The double value in the union is there to ensure alignment is
   * good for IEEE doubles too.  In many 32-bit environments 4 bytes
   * would be sufficiently aligned and the double value is unnecessary.
   */
  union {
    size_t sz;
    double d;
  } u;
} alloc_hdr;

void *sandbox_alloc(void *udata, duk_size_t size) {
  alloc_hdr *hdr;

  dukext_t *vm = (dukext_t *)udata;

  if (size == 0) {
    return NULL;
  }

  if (vm->stats.heap + size > vm->config.max_heap) {
    fprintf(stderr,
            "Sandbox maximum allocation size reached, %ld requested in "
            "sandbox_alloc\n",
            (long)size);
    fflush(stderr);
    return NULL;
  }

  hdr = (alloc_hdr *)malloc(size + sizeof(alloc_hdr));
  if (!hdr) {
    return NULL;
  }
  hdr->u.sz = size;
  vm->stats.heap += size;
  vm->stats.count++;
  // sandbox_dump_memstate();
  return (void *)(hdr + 1);
}

void *sandbox_realloc(void *udata, void *ptr, duk_size_t size) {
  alloc_hdr *hdr;
  size_t old_size;
  void *t;

  dukext_t *vm = (dukext_t *)udata;

  /* Handle the ptr-NULL vs. size-zero cases explicitly to minimize
   * platform assumptions.  You can get away with much less in specific
   * well-behaving environments.
   */

  if (ptr) {
    hdr = (alloc_hdr *)(((char *)ptr) - sizeof(alloc_hdr));
    old_size = hdr->u.sz;

    if (size == 0) {
      vm->stats.heap -= old_size;
      free((void *)hdr);
      // sandbox_dump_memstate();
      return NULL;
    } else {
      if (vm->stats.heap - old_size + size > vm->config.max_heap) {
        fprintf(stderr,
                "Sandbox maximum allocation size reached, %ld requested in "
                "sandbox_realloc\n",
                (long)size);
        fflush(stderr);
        return NULL;
      }

      t = realloc((void *)hdr, size + sizeof(alloc_hdr));
      if (!t) {
        return NULL;
      }
      hdr = (alloc_hdr *)t;
      vm->stats.heap -= old_size;
      vm->stats.heap += size;
      hdr->u.sz = size;
      // sandbox_dump_memstate();
      return (void *)(hdr + 1);
    }
  } else {
    if (size == 0) {
      return NULL;
    } else {
      if (vm->stats.heap + size > vm->config.max_heap) {
        fprintf(stderr,
                "Sandbox maximum allocation size reached, %ld requested in "
                "sandbox_realloc\n",
                (long)size);
        fflush(stderr);
        return NULL;
      }

      hdr = (alloc_hdr *)malloc(size + sizeof(alloc_hdr));
      if (!hdr) {
        return NULL;
      }
      hdr->u.sz = size;
      vm->stats.heap += size;
      // sandbox_dump_memstate();
      return (void *)(hdr + 1);
    }
  }
}

void sandbox_free(void *udata, void *ptr) {
  alloc_hdr *hdr;

  dukext_t *vm = (dukext_t *)udata; /* Suppress warning. */

  if (!ptr) {
    return;
  }
  hdr = (alloc_hdr *)(((char *)ptr) - sizeof(alloc_hdr));
  vm->stats.heap -= hdr->u.sz;
  vm->stats.count--;
  free((void *)hdr);
  // sandbox_dump_memstate();
}

void sandbox_fatal(void *udata, const char *msg) {
  (void)udata; /* Suppress warning. */
  fprintf(stderr, "FATAL: %s\n", (msg ? msg : "no message"));
  fflush(stderr);
  exit(1); /* must not return */
}
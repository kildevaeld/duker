#include "sandbox.h"
#include <string.h>

void *sandbox_alloc(void *udata, duk_size_t size) {}
void *sandbox_realloc(void *udata, void *ptr, duk_size_t size) {}
void sandbox_free(void *udata, void *ptr) {}

void sandbox_fatal(void *udata, const char *msg) {
  (void)udata; /* Suppress warning. */
  fprintf(stderr, "FATAL: %s\n", (msg ? msg : "no message"));
  fflush(stderr);
  exit(1); /* must not return */
}
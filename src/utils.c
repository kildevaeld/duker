#include "utils.h"

size_t cs_str_indexof(const char *path, char p) {
  int len = strlen(path);
  for (int i = 0; i < len; i++) {
    if (path[i] == p)
      return i;
  }

  return -1;
}
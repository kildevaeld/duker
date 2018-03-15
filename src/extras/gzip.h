#pragma once
#include <stdlib.h>

typedef struct cs_gzip_ctx_s {
  int len;
  uint32_t crc;
} cs_gzip_ctx_t;

void cs_gzip_write_hdr(unsigned char buffer[10]);
void cs_gzip_write_trail(cs_gzip_ctx_t *ctx, unsigned char buffer[8]);
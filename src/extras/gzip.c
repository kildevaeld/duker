#include "gzip.h"
// https://github.com/strake/gzip/blob/master/gzip.c

static void storLE32(uint8_t *p, uint32_t n) {
  p[0] = n >> 0;
  p[1] = n >> 8;
  p[2] = n >> 16;
  p[3] = n >> 24;
}

void cs_gzip_write_hdr(unsigned char buffer[10]) {
  uint8_t hdr[10] = {
      0x1F, 0x8B,       /* magic */
      8,                /* z method */
      0,                /* flags */
      0,    0,    0, 0, /* mtime */
      0,                /* xfl */
      0xFF,             /* OS */
  };
  memcpy(buffer, hdr, 10);
}
void cs_gzip_write_trail(cs_gzip_ctx_t *ctx, unsigned char buffer[8]) {
  uint8_t ftr[8];
  storLE32(ftr + 0, ctx->crc);
  storLE32(ftr + 4, ctx->len);
  memcpy(buffer, ftr, 8);
}
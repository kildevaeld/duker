#include "fs.h"
#include "bag.h"
#include "types.h"
#include <duker/refs.h>

static void on_read_cb(uv_fs_t *req);

struct fbag {
  duk_context *ctx;
  int ref;
  int bref;

  uv_buf_t output;
  size_t cursor;
  uv_buf_t buffer;
  ssize_t fd;
};

static inline void clean_fbag(struct fbag *b) {
  if (!b)
    return;
  free(b->buffer.base);
  dk_unref(b->ctx, b->ref);
  dk_unref(b->ctx, b->bref);
  b->ctx = NULL;
  free(b);
}

static void on_close_cb(uv_fs_t *req) {
  struct fbag *b = req->data;
  // push callback;
  dk_push_ref(b->ctx, b->ref);

  if (req->result < 0) {
    duk_push_error_object(b->ctx, DUK_ERR_ERROR, uv_strerror(req->result));
  } else {
    duk_push_undefined(b->ctx);
  }

  dk_push_ref(b->ctx, b->bref);

  duk_call(b->ctx, 2);
  duk_pop(b->ctx);

  clean_fbag(b);
  uv_fs_req_cleanup(req);
  free(req);
}

static void on_read_cb(uv_fs_t *req) {

  struct fbag *b = req->data;

  if (req->result < 0) {
    dk_push_ref(b->ctx, b->ref);
    duk_push_error_object(b->ctx, DUK_ERR_ERROR, uv_strerror(req->result));
    duk_push_undefined(b->ctx);
    duk_call(b->ctx, 2);
    duk_pop(b->ctx);
    clean_fbag(b);
  } else if (req->result == 0) {
    uv_fs_t *r = malloc(sizeof(uv_fs_t));
    r->data = req->data;
    uv_fs_close(req->loop, r, b->fd, on_close_cb);
  } else if (req->result > 0) {
    uv_fs_t *r = malloc(sizeof(uv_fs_t));
    r->data = req->data;

    if (b->cursor + req->result >= b->output.len) {
      dk_push_ref(b->ctx, b->bref);
      b->output.base =
          duk_resize_buffer(b->ctx, -1, b->output.len + req->result + 1024);
      b->output.len += req->result + 1024;
      duk_pop(b->ctx);
    }

    memcpy(b->output.base + b->cursor, b->buffer.base, req->result);
    b->cursor += req->result;

    uv_fs_read(req->loop, r, b->fd, &b->buffer, 1, b->cursor, on_read_cb);
  }

  uv_fs_req_cleanup(req);
  free(req);
}

static void on_open_cb(uv_fs_t *req) {
  struct fbag *b = req->data;
  if (req->result >= 0) {

    uv_fs_t *r = malloc(sizeof(uv_fs_t));
    r->data = req->data;
    b->fd = req->result;
    uv_fs_read(req->loop, r, req->result, &b->buffer, 1, -1, on_read_cb);
  } else {
    dk_push_ref(b->ctx, b->ref);
    duk_push_error_object(b->ctx, DUK_ERR_ERROR, uv_strerror(req->result));
    duk_push_undefined(b->ctx);
    duk_call(b->ctx, 2);
    duk_pop(b->ctx);
    clean_fbag(b);
  }
  uv_fs_req_cleanup(req);
  free(req);
}

static duk_ret_t fs_read_file(duk_context *ctx) {

  const char *path = duk_require_string(ctx, 0);
  duk_idx_t idx = DUK_INVALID_INDEX;
  if (duk_is_function(ctx, 1)) {
    idx = 1;
  }

  if (idx == DUK_INVALID_INDEX) {
    duk_type_error(ctx, "no callback");
  }

  duk_dup(ctx, idx);
  int ref = dk_ref(ctx);

  char *ptr = duk_push_dynamic_buffer(ctx, sizeof(char) * 1024);

  int bidx = dk_ref(ctx);

  uv_loop_t *loop = dk_stash_get_ptr(ctx, kStashLoopKey);

  uv_fs_t *req = malloc(sizeof(uv_fs_t));

  struct fbag *b = malloc(sizeof(struct fbag));
  b->ctx = ctx;
  b->ref = ref;
  // b->output = ptr;
  // b->output_len = 128;
  b->cursor = 0;
  b->buffer.base = malloc(sizeof(char) * 1024);
  b->buffer.len = 1024;
  b->output.base = ptr;
  b->output.len = 1024;

  b->bref = bidx;

  req->data = b;

  uv_fs_open(loop, req, path, O_RDONLY, 0, on_open_cb);

  return 0;
}

int fs_init_module(duk_context *ctx) {

  duk_push_object(ctx);

  duk_push_c_function(ctx, fs_read_file, 3);
  duk_put_prop_string(ctx, -2, "readFile");

  return 1;
}

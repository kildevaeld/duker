#include <duker/duker.h>
#include <stdio.h>
#include <unity.h>

void test_path_join() {
  duker_t *ctx = dk_create(NULL);
  TEST_ASSERT_NOT_NULL(ctx);

  dk_add_default_modules(ctx);

  // dk_eval_script(ctx, "", "require('path').join('test', '/test', '/mig/');");

  duk_context *c = dk_duk_context(ctx);

  // duk_eval_string(c, "require('path').join('test', '/test', '/mig/');");
  dk_eval_script(ctx, "rapper",
                 "require('path').join('test', '/test', '/mig/');");

  const char *path = duk_require_string(c, -1);
  TEST_ASSERT_EQUAL_STRING("test/test/mig", path);

  dk_free(ctx);
}

int main() {

  UNITY_BEGIN();
  RUN_TEST(test_path_join);
  return UNITY_END();
}
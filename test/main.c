#include <dukext/dukext.h>
#include <dukext/module.h>

#include <stdio.h>
#include <unity.h>

void test_script_module() {
  dukext_t *vm = dukext_create_default();

  dukext_module_string_set(vm, "test", "exports.test = 'Hello, World'");
  dukext_err_t *err = NULL;
  dukext_eval_script(vm, "console.log(require('test').test);", "path", &err);
  if (err) {
    printf("error %s\n", err->message);
    dukext_err_free(err);
  }
}

int main() {

  UNITY_BEGIN();
  RUN_TEST(test_script_module);
  return UNITY_END();
}
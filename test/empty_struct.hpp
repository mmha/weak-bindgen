// RUN: %weak-bindgen %s -l libfoo.so -o - | FileCheck %s
#include "header.hpp"

struct str {};

// CHECK: #include "foo.h"

// CHECK: static void *module_handle;
// CHECK: bool const libfoo_available = [] {
// CHECK-NEXT:     module_handle = ::dlopen("libfoo.so", RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
// CHECK-NEXT:     return module_handle;
// CHECK-NEXT: }();

// CHECK-NOT: should_be_invisible

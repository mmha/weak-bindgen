// RUN: not %weak-bindgen %s -l libfoo.so -o /dev/null 2>&1 | FileCheck %s

void f();
void vararg(...);    // CHECK: varargs.hpp:[[@LINE]]:6: error: Vararg functions cannot be forwarded
void g();

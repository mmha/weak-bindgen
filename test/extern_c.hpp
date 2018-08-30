// RUN: %weak-bindgen %s -l libfoo.so -o - | FileCheck %s

extern "C" void c_linkage();
// CHECK: void c_linkage() {
// CHECK: ::dlsym(module_handle, "c_linkage")

extern "C" float with_args(int, char);
// CHECK: float with_args(int p0, char p1) {
// CHECK: ::dlsym(module_handle, "with_args")

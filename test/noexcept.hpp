// RUN: %weak-bindgen %s -l libfoo.so -o - | FileCheck %s

void a() noexcept;    // CHECK: void a() noexcept {

constexpr bool true_() {
	return true;
}

void b() noexcept(true_());    // CHECK: void b() noexcept {

void c(void (*)() noexcept) noexcept(false);    // CHECK: void c(id<void (*)() noexcept> p0) {

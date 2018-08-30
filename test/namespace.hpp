// RUN: %weak-bindgen %s -l libfoo.so -o - | FileCheck %s
namespace N {
	void a();    // CHECK: void N::a() {
	struct return_type {
		int member;
	};

	return_type b();    // CHECK: N::return_type N::b() {
}    // namespace N

N::return_type c();    // CHECK: N::return_type c() {

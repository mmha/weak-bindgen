// RUN: %weak-bindgen %s -l libfoo.so -o - | FileCheck %s

void *a();         // CHECK: void * a() {
void b(void *);    // CHECK: void b(void * p0) {

void c(void (*)());    // CHECK: void c(id<void (*)()> p0) {
void d(void (&)());    // CHECK: void d(id<void (&)()> p0) {

struct S {
	void f();
	int i;
};

void e(void (S::*)());    // CHECK: void e(id<void (S::*)()> p0) {
void f(int S::*);         // CHECK: void f(id<int S::*> p0) {

void g(void (**)());    // CHECK: void g(id<void (**)()> p0) {

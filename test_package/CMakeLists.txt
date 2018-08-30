cmake_minimum_required(VERSION 3.11)
project(weak-bindgen-integration-test VERSION 1.0)

include("${CMAKE_BINARY_DIR}/conan_paths.cmake")

find_package(weak-bindgen 0.1 REQUIRED)

execute_process(COMMAND weak-bindgen::weak-bindgen -version)

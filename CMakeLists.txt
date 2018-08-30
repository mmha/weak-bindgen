cmake_minimum_required(VERSION 3.11)
project(weak-bindgen VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include("${CMAKE_BINARY_DIR}/conan_paths.cmake" OPTIONAL)
include(AddCompileOptionIfSupported)
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

add_compile_options_if_supported(
    -Wall -Wextra -Wpedantic -Wconversion
    -Wthread-safety -Wdocumentation
    -fdiagnostics-color=always
    -fdiagnostics-show-template-tree)

find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)

add_subdirectory(src bin)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/weak-bindgen-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

export(EXPORT weak-bindgen-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/weak-bindgen-targets.cmake"
  NAMESPACE weak-bindgen::)

configure_file(cmake/weak-bindgen-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/weak-bindgen-config.cmake"
  @ONLY)

install(EXPORT weak-bindgen-targets
  FILE weak-bindgen-targets.cmake
  NAMESPACE weak-bindgen::
  DESTINATION lib/cmake/weak-bindgen)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/weak-bindgen-config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/weak-bindgen-config-version.cmake"
        DESTINATION lib/cmake/weak-bindgen)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(llvm_version "${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH}")
set(WEAKBINDGEN_CLANG_INTERNAL_INCLUDE_PATH "${CLANG_INSTALL_PREFIX}/lib/clang/${llvm_version}/include")
set(WEAKBINDGEN_CLANG_INCLUDE_PATH "${CLANG_INSTALL_PREFIX}/include")
configure_file(include_paths.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include_paths.hpp" @ONLY)

add_executable(weak-bindgen
    CodeGen.hpp
    CXXCodeGen.hpp CXXCodeGen.cpp
    FunctionDeclarationReplacer.hpp FunctionDeclarationReplacer.cpp
    main.cpp)
target_include_directories(weak-bindgen SYSTEM PRIVATE ${CLANG_INCLUDE_DIRS} ${LLVM_INCLUDE_DIR})
target_link_libraries(weak-bindgen PRIVATE clangAST clangBasic clangFrontend clangTooling clangIndex LLVMCore LLVMSupport)
target_compile_features(weak-bindgen PRIVATE cxx_std_17)
target_compile_options(weak-bindgen PRIVATE -fno-rtti)

install(TARGETS weak-bindgen
        EXPORT weak-bindgen-targets
        RUNTIME DESTINATION bin)

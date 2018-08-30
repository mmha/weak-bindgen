from conans import ConanFile, CMake, RunEnvironment, tools
import os

class WeakBindgen(ConanFile):
    name = "weak-bindgen"
    version = "0.1"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake_paths", "virtualrunenv"
    requires = "cmake_installer/[>=3.11.3]@conan/stable", "clang/6.0.0@Manu343726/testing"
    exports_sources = "CMakeLists.txt", ".clang*", "test/*", "src/*", "cmake/*"
    no_copy_source = True

    def build(self):
        cmake = CMake(self)
        if self.should_configure:
            cmake.configure(source_dir = self.source_folder)
        if self.should_build:
            cmake.build()
        if self.should_test:
            env_build = RunEnvironment(self)
            with tools.environment_append(env_build.vars):
                cmake.test()
        if self.should_install:
            cmake.install()

    def package_info(self):
        self.env_info.PATH.append(os.path.join(self.package_folder, "bin"))

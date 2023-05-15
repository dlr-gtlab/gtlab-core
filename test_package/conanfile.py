
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run
import os


class TestPackageConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain", "virtualenv"

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):

            for exec in ["dataprocessor_test", "core_test", "gui_test"]:
                bin_path = os.path.join(self.cpp.build.bindirs[0], exec)
                self.run(bin_path, run_environment=True)

        print("\nAll conan gtlab tests passed!")


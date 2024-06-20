# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools import files
import os   

required_conan_version = ">=1.45.0"

class GTlabCore(ConanFile):
    name = "gtlab-core"
    version = "2.0.9"
    license = "GTlab license"
    author = "Martin Siggel <martin.siggel@dlr.de>"
    url = "https://gitlab.dlr.de/gtlab-coredev/gtlab"
    homepage = "https://gitlab.dlr.de/gtlab-coredev/gtlab"
    toppics = "core"
    description = "The gtlab framework"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "build": ["stable", "unstable"], "with_hdf5": [True, False]}
    default_options = {"shared": True, "fPIC": True, "build": "unstable",  "with_hdf5": True}
    exports_sources = "src/*", "cmake/*", "CHANGELOG.md", "translations/*", "CMakeLists.txt", "README.md"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_WITH_HDF5"] = self.options.with_hdf5
        tc.generate()
        
        CMakeDeps(self).generate()
  
  
    def requirements(self):
        if self.options.build  == "stable":
            self.requires("qt/5.12.5@dlr-at/stable")
            self.requires("gtlab-logging/4.1.0")
        else:
            self.requires("qt/5.15.2")
            self.requires("gtlab-logging/4.1.0")
            
            if self.options.with_hdf5:
                self.requires("genh5/3.0.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        self.options["gtlab-logging"].shared = self.options.shared
        self.options["qt"].qtsvg = True
        self.options["qt"].qtdeclarative = True # for Qml
        self.options["qt"].shared = True

    def build(self):    
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

        files.rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        files.copy(self, "README.md", dst=os.path.join(self.package_folder, "licenses"), src=self.source_folder)

    def package_info(self):
        
        self.cpp_info.names["cmake_find_package"] = "GTlab"
        self.cpp_info.names["cmake_find_package_multi"] = "GTlab"

        self.cpp_info.set_property("cmake_file_name", "GTlab")
        self.cpp_info.set_property("cmake_target_name", "GTlab")


        def define_lib(name, lib, requires):
            self.cpp_info.components[name].includedirs.append(os.path.join("include", "core"))

            self.cpp_info.components[name].set_property("cmake_target_name", "GTlab::" + name)
            self.cpp_info.components[name].requires = requires
        

            if self.settings.build_type != "Debug":
                self.cpp_info.components[name].libs = [lib]
            else:
                self.cpp_info.components[name].libs = [lib+"-d"]

        define_lib("DataProcessor", "GTlabDataProcessor", ["qt::qt", "gtlab-logging::gtlab-logging"])
        define_lib("Core", "GTlabCore", ["DataProcessor"])
        define_lib("Gui", "GTlabGui", ["DataProcessor", "Core"])
        
        # if built as static libs, the users must use those compile definitions
        if not self.options.shared:
            self.cpp_info.components["DataProcessor"].defines = ['GT_DATAMODEL_LIB']
            self.cpp_info.components["Core"].defines = ['GT_CORE_LIB']
            self.cpp_info.components["Gui"].defines = ['GT_GUI_LIB']


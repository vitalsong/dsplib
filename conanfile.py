from conan import ConanFile
from conans.tools import load
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
import re

def get_version():
    try:
        content = load("CMakeLists.txt")
        version = re.search("project\(dsplib .* VERSION (.*)\)", content).group(1)
        return version.strip()
    except Exception as e:
        return None

class DsplibConan(ConanFile):
    name = "dsplib"
    version = get_version()

    license = "MIT"
    author = "Vitaly Yulis (vitaly.yulis@gmail.com)"
    url = "https://github.com/vitalsong/dsplib"
    description = "C++ DSP library for MATLAB/Octave similar programming"
    topics = ("dsp", "matlab", "c++17", "sound", "radio")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "float32": [True, False]}
    default_options = {"shared": False, "fPIC": True, "float32": False}

    exports_sources = "cmake/*", "CMakeLists.txt", "lib/*", "include/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables = {
            "BUILD_SHARED_LIBS": "ON" if self.options.shared else "OFF",
            "DSPLIB_USE_FLOAT32": "ON" if self.options.float32 else "OFF",
            "CMAKE_BUILD_TYPE": self.settings.build_type})
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = ["dsplib"]

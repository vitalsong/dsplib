from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import check_min_cppstd
import re

required_conan_version = ">=2.1.0"


def get_version():
    try:
        with open("CMakeLists.txt", "r", encoding="utf-8", newline="") as handle:
            content = handle.read()
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
    topics = ("dsp", "matlab", "c++17", "audio")

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "float32": [True, False],
        "noexcept": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
        "float32": False,
        "noexcept": False,
    }

    generators = ["CMakeDeps"]

    exports_sources = "cmake/*", "CMakeLists.txt", "lib/*", "include/*"
    
    def validate(self):
        check_min_cppstd(self, 17)
        
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["DSPLIB_USE_FLOAT32"] = self.options.float32
        tc.variables["DSPLIB_NO_EXCEPTIONS"] = self.options.noexcept
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        if self.options.get_safe("fPIC"):
            tc.variables["CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.fPIC
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = ["dsplib"]

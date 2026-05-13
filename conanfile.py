from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import check_min_cppstd

required_conan_version = ">=2.1.0"


class DsplibConan(ConanFile):
    name = "dsplib"
    version = "1.2.0"
    license = "MIT"
    package_type = "library"
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
        "fft": ["dsplib", "fftw", "ne10"],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
        "float32": True,
        "noexcept": False,
        "fft": "dsplib",
    }

    generators = ["CMakeDeps"]

    exports_sources = "cmake/*", "CMakeLists.txt", "lib/*", "include/*"

    def validate(self):
        check_min_cppstd(self, 17)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
            
    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
        
    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["DSPLIB_USE_FLOAT32"] = self.options.float32
        tc.variables["DSPLIB_NO_EXCEPTIONS"] = self.options.noexcept
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["DSPLIB_FFT_BACKEND"] = self.options.fft
        if self.options.get_safe("fPIC"):
            tc.variables["CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.fPIC
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def requirements(self):
        if self.options.fft == "fftw":
            self.requires("fftw/3.3.10")
        elif self.options.fft == "ne10":
            # NE10 is not available in ConanCenter.
            # It can be resolved from a private remote or a locally exported recipe.
            self.requires("ne10/1.2.1")
        
    def build_requirements(self):
        self.tool_requires("cmake/[>=3.23 <4]")
        self.tool_requires("ninja/[>=1.10.2 <2]")
        
        # for benchmarks/tests only
        if self.options.fft != "fftw":
            self.test_requires("fftw/3.3.10")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = ["dsplib"]
        if self.options.fft == "fftw":
            self.cpp_info.requires = ["fftw::fftw"]
        if self.options.fft == "ne10":
            self.cpp_info.requires = ["ne10::ne10"]
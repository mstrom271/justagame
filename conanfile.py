import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

class Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        pass

    def build_requirements(self):
        self.tool_requires("cmake/3.26.4")
        self.tool_requires("ninja/1.11.1")

    def layout(self):
        # cmake_layout(self)
        self.folders.source = "."
        self.folders.build = "."
        self.folders.generators = "./generators"
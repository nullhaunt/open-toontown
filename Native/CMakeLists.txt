cmake_minimum_required(VERSION 3.18)
project(ottoon_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python 3.9 REQUIRED COMPONENTS Interpreter Development.Module)

include(FetchContent)
FetchContent_Declare(
  nanobind
  GIT_REPOSITORY https://github.com/wjakob/nanobind.git
  GIT_TAG        v2.12.0
)
FetchContent_MakeAvailable(nanobind)

nanobind_add_module(ottoon Source/SeamTest.cpp)
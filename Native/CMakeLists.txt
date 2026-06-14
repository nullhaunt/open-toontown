cmake_minimum_required(VERSION 3.18)
project(toontown_cpp_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python 3.9 REQUIRED COMPONENTS Interpreter Development.Module)

include(FetchContent)
FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind.git
        GIT_TAG v2.12.0
)
FetchContent_MakeAvailable(nanobind)

nanobind_add_module(toontown_cpp
        Source/Module.cpp
        Source/Toontown/Minigame/DropScheduler.cpp
        Source/Toontown/SafeZone/Checkerboard.cpp
        Source/Toontown/Math/MathBoundary.cpp
        Source/Toontown/ClassicChars/CCharPaths.cpp
        Source/Toontown/Suit/SuitDNA.cpp
)

target_include_directories(toontown_cpp PRIVATE Include Source)

set_target_properties(toontown_cpp PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binary"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Binary"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Binary"
)
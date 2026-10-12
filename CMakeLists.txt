cmake_minimum_required(VERSION 3.30)

project(parallel_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")

add_executable(matrix main.cpp)

target_compile_options(matrix PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
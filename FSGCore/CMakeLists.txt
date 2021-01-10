# CMake Metadata
cmake_minimum_required(VERSION 3.17)
project(fsgcore)

# FSGCore Include Directory
include_directories(Include)

# Files
file(GLOB_RECURSE FSGCORE ${CMAKE_SOURCE_DIR} "**/*.cpp")

# Building
add_library(fsgcore ${FSGCORE})
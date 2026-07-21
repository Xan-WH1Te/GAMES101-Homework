cmake_minimum_required(VERSION 3.10)
project(Transformation)

# Find Eigen3 (installed system-wide via apt)
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})

add_executable(Transformation main.cpp)

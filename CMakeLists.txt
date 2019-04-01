cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(h5cpp)

find_package(HDF5 REQUIRED)

include_directories(${HDF5_INCLUDE_DIRS})

add_library(h5cpp h5cpp/h5reader.cpp)

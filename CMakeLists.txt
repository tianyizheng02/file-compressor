cmake_minimum_required(VERSION 3.19)
project(file_compressor)

set(CMAKE_CXX_STANDARD 20)

add_executable(file_compressor compressor.cpp DLB.cpp DLB.h)
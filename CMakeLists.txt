cmake_minimum_required(VERSION 3.29)
project(ray_tracer)

set(CMAKE_CXX_STANDARD 20)

add_executable(ray_tracer main.cpp
        includes/intersection.h
        includes/tests.h)

target_include_directories (ray_tracer PUBLIC includes)
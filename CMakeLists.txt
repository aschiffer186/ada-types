cmake_minimum_required(VERSION 3.25)
project(cina LANGUAGES CXX)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_MODULE "Build the library as a C++20 module" OFF)

add_subdirectory(include)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
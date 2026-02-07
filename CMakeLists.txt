cmake_minimum_required(VERSION 3.27)
project(cina LANGUAGES CXX)

add_subdirectory(include)

enable_testing()
add_subdirectory(tests)
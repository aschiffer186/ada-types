cmake_minimum_required(VERSION 3.27)
project(ada_typing LANGUAGES CXX)

add_subdirectory(include)

enable_testing()
add_subdirectory(tests)
find_package(GTest REQUIRED)

add_executable(test_boolean ${CMAKE_CURRENT_SOURCE_DIR}/test_boolean.cpp)
target_link_libraries(test_boolean PRIVATE GTest::gtest_main ${PROJECT_NAME})
gtest_discover_tests(test_boolean)
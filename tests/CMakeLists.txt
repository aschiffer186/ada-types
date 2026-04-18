find_package(GTest REQUIRED)

add_executable(test_boolean ${CMAKE_CURRENT_SOURCE_DIR}/test_boolean.cpp)
target_link_libraries(test_boolean PRIVATE GTest::gtest_main ${PROJECT_NAME})
gtest_discover_tests(test_boolean)

add_executable(test_signed_integer ${CMAKE_CURRENT_SOURCE_DIR}/test_signed_integer_type.cpp)
target_link_libraries(test_signed_integer PRIVATE GTest::gtest_main ${PROJECT_NAME})
gtest_discover_tests(test_signed_integer)
find_package(GTest REQUIRED)

add_executable(compile_check compile_check.cpp)
target_link_libraries(compile_check PRIVATE ${PROJECT_NAME})

add_executable(test_boolean test_boolean.cpp)
target_link_libraries(test_boolean PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_boolean)

add_executable(test_integer_type test_integer_type.cpp)
target_link_libraries(test_integer_type PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_integer_type)
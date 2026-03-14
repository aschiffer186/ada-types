find_package(GTest REQUIRED)

add_executable(test_boolean test_boolean.cpp)
target_link_libraries(test_boolean PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_boolean)

add_executable(test_integer_type test_integer_type.cpp)
target_link_libraries(test_integer_type PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_integer_type)

add_executable(test_bitwise_integer_type test_bitwise_integer_type.cpp)
target_link_libraries(test_bitwise_integer_type PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_bitwise_integer_type)

add_executable(test_strong_type_base test_strong_type_base.cpp)
target_link_libraries(test_strong_type_base PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_strong_type_base)
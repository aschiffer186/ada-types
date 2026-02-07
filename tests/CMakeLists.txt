find_package(GTest REQUIRED)

add_executable(compile_check compile_check.cpp)
target_link_libraries(compile_check PRIVATE ${PROJECT_NAME})

add_executable(test_type_factory test_type_factory.cpp)
target_link_libraries(test_type_factory PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_type_factory)

add_executable(test_boolean test_boolean.cpp)
target_link_libraries(test_boolean PRIVATE ${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(test_boolean)
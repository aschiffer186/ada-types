find_package(GTest REQUIRED)

add_executable(compile_check compile_check.cpp)
target_link_libraries(compile_check PRIVATE ada_types)

add_executable(test_strong_type test_strong_type.cpp)
target_link_libraries(test_strong_type PRIVATE ada_types GTest::gtest_main)
add_test(NAME TestStrongType COMMAND test_strong_type)
gtest_discover_tests(test_strong_type)

add_executable(test_signed_integer test_signed_integer.cpp)
target_link_libraries(test_signed_integer PRIVATE ada_types GTest::gtest_main)
add_test(NAME TestSignedInteger COMMAND test_signed_integer)
gtest_discover_tests(test_signed_integer)

add_executable(test_compilation_failure1 test_compilation_failures.cpp)
set_target_properties(test_compilation_failure1 PROPERTIES EXCLUDE_FROM_ALL TRUE EXCLUDE_FROM_DEFAULT_BUILD TRUE)
target_compile_definitions(test_compilation_failure1 PRIVATE TEST_INCOMPATIBLE_STRONG_TYPES)
target_link_libraries(test_compilation_failure1 PRIVATE ada_types)
add_test(NAME TestIncompatibleStrongTypes 
        COMMAND ${CMAKE_COMMAND} --build . --target test_compilation_failure1 --config $<CONFIGURATION>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(TestIncompatibleStrongTypes PROPERTIES WILL_FAIL TRUE)

add_executable(test_compilation_failure2 test_compilation_failures.cpp)
set_target_properties(test_compilation_failure2 PROPERTIES EXCLUDE_FROM_ALL TRUE EXCLUDE_FROM_DEFAULT_BUILD TRUE)
target_compile_definitions(test_compilation_failure2 PRIVATE TEST_CHECKED_INTEGER_DEFAULT_CONSTRUCTOR1)
target_link_libraries(test_compilation_failure2 PRIVATE ada_types)
add_test(NAME TestSignedIntegerBadDefaultConstructor1 
        COMMAND ${CMAKE_COMMAND} --build . --target test_compilation_failure2 --config $<CONFIGURATION>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(TestSignedIntegerBadDefaultConstructor1  PROPERTIES WILL_FAIL TRUE)

add_executable(test_compilation_failure3 test_compilation_failures.cpp)
set_target_properties(test_compilation_failure3 PROPERTIES EXCLUDE_FROM_ALL TRUE EXCLUDE_FROM_DEFAULT_BUILD TRUE)
target_compile_definitions(test_compilation_failure3 PRIVATE TEST_CHECKED_INTEGER_DEFAULT_CONSTRUCTOR1)
target_link_libraries(test_compilation_failure3 PRIVATE ada_types)
add_test(NAME TestSignedIntegerBadDefaultConstructor2 
        COMMAND ${CMAKE_COMMAND} --build . --target test_compilation_failure3 --config $<CONFIGURATION>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(TestSignedIntegerBadDefaultConstructor2  PROPERTIES WILL_FAIL TRUE)
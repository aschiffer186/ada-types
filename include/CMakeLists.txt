if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.28" AND BUILD_MODULE) 
add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES ${CMAKE_CURRENT_SOURCE_DIR}/cina.cppm
)
target_compile_features(${PROJECT_NAME}
    PUBLIC cxx_std_20
)
target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_MODULE)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
else()
add_library(${PROJECT_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cina.hpp
)
target_include_directories(${PROJECT_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_features(${PROJECT_NAME}
    INTERFACE cxx_std_20
)
endif()
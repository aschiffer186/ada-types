add_library(ada_types INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/ada_types.hpp
)
target_include_directories(ada_types 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
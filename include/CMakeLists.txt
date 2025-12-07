add_library(ada_types 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/strong_types.hpp
)
target_include_directories(ada_types 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)  
target_compile_features(ada_types 
    INTERFACE cxx_std_23
)
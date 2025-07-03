add_library(clang SHARED IMPORTED GLOBAL)

if(UNIX)
    set_target_properties(clang PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux/libclang.so.12
    )
endif()

target_include_directories(clang INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
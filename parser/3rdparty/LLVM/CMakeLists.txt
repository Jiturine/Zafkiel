add_library(clang SHARED IMPORTED GLOBAL)

if(UNIX)
    set_target_properties(clang PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux/libclang.so.20.1
    )
endif()
if(WIN32)
    set_target_properties(clang PROPERTIES
        IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/bin/libclang.dll
        IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libclang.lib
    )
endif()

target_include_directories(clang INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(mono STATIC IMPORTED GLOBAL)

if(UNIX)
    set_target_properties(mono PROPERTIES
        IMPORTED_LOCATION  ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/libmonosgen-2.0.a
    )
endif()

if(WIN32)
    set_target_properties(mono PROPERTIES
        IMPORTED_LOCATION  ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/mono-2.0-sgen.lib
    )
endif()

target_include_directories(mono INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
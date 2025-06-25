add_library(mono STATIC IMPORTED GLOBAL)

set_target_properties(mono PROPERTIES
    IMPORTED_LOCATION  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmonosgen-2.0.a
)

target_include_directories(mono INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
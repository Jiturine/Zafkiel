add_library(vulkan SHARED IMPORTED GLOBAL)
set_target_properties(vulkan PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libvulkan.so.1
)

add_library(shaderc_shared SHARED IMPORTED)
set_target_properties(shaderc_shared PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libshaderc_shared.so.1
)

add_library(spirv_cross INTERFACE)
target_link_libraries(spirv_cross INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libspirv-cross-core.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libspirv-cross-reflect.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libspirv-cross-glsl.a
)

target_include_directories(vulkan INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
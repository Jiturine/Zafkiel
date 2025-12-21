# 查找所有源文件
file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c )

# 查找需要解析反射的文件
file(GLOB_RECURSE reflection_files 
${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# 将reflection_files的内容写入txt，方便parser读取
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/precompile/refl_files.txt.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/precompile/refl_files.txt
)

include(${CMAKE_SOURCE_DIR}/parser/parse.cmake)
parse_module(
    Engine 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/meta/reflection/refl.h
)

# generated文件为parser自动生成
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/engine_refl_generate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/engine_refl_generate.cpp
    PROPERTIES GENERATED TRUE
)

add_library(engine STATIC 
    ${HEADER_FILES}
    ${SOURCE_FILES} 
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/engine_refl_generate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/engine_refl_generate.cpp
)

set_target_properties(engine PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/engine
)

# 添加预编译头文件
target_precompile_headers(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/precompile/pch.h )

target_include_directories(engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/generated)

# 链接第三方库
target_link_libraries(engine PUBLIC spdlog mono z EnTT SDL3::SDL3 glad glm yaml-cpp stb_image assimp vulkan shaderc_shared spirv_cross imgui glslang glslang-default-resource-limits)
if(WIN32)
    target_link_libraries(engine PUBLIC opengl32)
elseif(UNIX)
    target_link_libraries(engine PUBLIC GL)
endif()

add_custom_target(
    engine_refl_dependency
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated/engine_refl_generate.cpp
)

# 让engine目标依赖反射代码生成
add_dependencies(engine engine_refl_dependency)

add_subdirectory(script_core)

add_dependencies(engine script_core)
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
    Editor 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/engine/src/Core/Meta/Reflection/Refl.h
)

# generated文件为parser自动生成
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/EditorReflGenerate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/EditorReflGenerate.cpp
    PROPERTIES GENERATED TRUE
)

add_executable(editor 
    ${HEADER_FILES} 
    ${SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/EditorReflGenerate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/EditorReflGenerate.cpp
)

set_target_properties(editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/editor
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_BINARY_DIR}/bin/editor
)

target_include_directories(editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_precompile_headers(editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../engine/precompile/pch.h )

target_link_libraries(editor PUBLIC engine imguizmo filewatch)

add_custom_target(
    editor_refl_dependency
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated/editor_refl_generate.cpp
)

# 让editor目标依赖反射代码生成
add_dependencies(editor editor_refl_dependency)

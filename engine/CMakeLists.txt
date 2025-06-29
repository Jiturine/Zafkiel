add_subdirectory(parser)

add_subdirectory(runtime)

add_subdirectory(script_core)

# 查找需要解析反射的文件
file(GLOB_RECURSE reflection_files 
${CMAKE_CURRENT_SOURCE_DIR}/runtime/src/*.h
${CMAKE_CURRENT_SOURCE_DIR}/runtime/src/*.hpp
)

# 将reflection_files的内容写入txt，方便parser读取
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime/src/precompile/refl_files.txt.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime/src/precompile/refl_files.txt)
    
# 调用Parser解析反射文件
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/reflection_generated.done  # 标记文件
    COMMAND $<TARGET_FILE:Parser>
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime/src/precompile/refl_files.txt # 第一个参数：记录了所有需要解析的文件路径的txt文件路径
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime/generated    # 第二个参数：输出目录
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime/src/reflection/refl.h # 第三个参数：反射宏文件所在路径
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/reflection_generated.done  # 生成标记文件
    DEPENDS Parser ${reflection_files}  # 依赖 Parser 可执行文件和输入文件
    COMMENT "Generating reflection code..."
)
    
add_custom_target(
    refl_code_generate
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/reflection_generated.done  # 依赖标记文件
)

# 让 Zafkiel 依赖 refl_code_generate
add_dependencies(Zafkiel refl_code_generate)

add_dependencies(Zafkiel ScriptCore)
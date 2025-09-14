macro(parse_module module_name module_dir refl_macro_file)
    string(TOLOWER ${module_name} module_name_lower)
    add_custom_command(
        OUTPUT ${module_dir}/generated/${module_name_lower}_refl_generate.cpp    # 标记文件
        WORKING_DIRECTORY $<TARGET_FILE_DIR:parser>
        COMMAND $<TARGET_FILE:parser>
            ${module_name}                          # 项目名
            ${module_dir}/precompile/refl_files.txt # 记录了所有需要解析的文件路径的txt文件路径
            ${module_dir}/generated                 # 输出目录
            ${module_dir}                           # 根目录
            ${refl_macro_file}                      # 反射宏文件所在路径
        DEPENDS parser ${refl_macro_file}          # 依赖 parser 可执行文件和输入文件
        COMMENT "Generating reflection code..."
    )
endmacro()



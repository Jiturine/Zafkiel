add_subdirectory(3rdparty/LLVM)

file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)

add_executable(parser ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/parser
)

target_include_directories(parser PUBLIC 
    3rdparty/mustache
)

# 把mustache模板文件拷贝到运行目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates
    DESTINATION ${CMAKE_BINARY_DIR}/bin/parser
)

add_custom_command(
    TARGET parser PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/runtime/generated
    COMMENT "Removing file at build time"
)

target_link_libraries(parser PUBLIC clang)

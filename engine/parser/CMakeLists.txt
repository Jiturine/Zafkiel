set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)

# 需要安装Clang
find_package(Clang REQUIRED CONFIG)

add_executable(Parser ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(Parser PUBLIC 
    3rdparty/mustache
    ${CLANG_INCLUDE_DIRS}
)

# 把mustache模板文件拷贝到运行目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates
    DESTINATION ${CMAKE_BINARY_DIR}/engine/parser
)

add_custom_command(
    TARGET Parser PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/runtime/generated
    COMMENT "Removing file at build time"
)

target_link_libraries(Parser PUBLIC 
    clangAST
    clangBasic
    clangFrontend
    clangSerialization
    clangTooling
)

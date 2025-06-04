set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)

# 需要安装Clang 和 LLVM
find_package(Clang REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

add_executable(Parser ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(Parser PUBLIC 
    3rdparty/mustache
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
)

# 把mustache模板文件拷贝到运行目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates
    DESTINATION ${CMAKE_BINARY_DIR}/engine/parser
)

target_link_libraries(Parser PUBLIC 
    clangAST
    clangBasic
    clangFrontend
    clangSerialization
    clangTooling
)

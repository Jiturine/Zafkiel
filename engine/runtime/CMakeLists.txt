set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

# 查找所有源文件
file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c )

# generated文件为Parser自动生成
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/refl_generate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/refl_generate.cpp
    PROPERTIES GENERATED TRUE
)

add_library(Zafkiel STATIC 
    ${HEADER_FILES}
    ${SOURCE_FILES} 
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/refl_generate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/refl_generate.cpp
)

set_target_properties(Zafkiel PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/Zafkiel
)

# 添加预编译头文件
target_precompile_headers(Zafkiel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/precompile/pch.h )

target_include_directories(Zafkiel PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/generated)

# 链接第三方库
target_link_libraries(Zafkiel PUBLIC spdlog mono EnTT SDL3::SDL3 glad glm yaml-cpp stb_image)
if(WIN32)
    target_link_libraries(Zafkiel PUBLIC opengl32)
elseif(UNIX)
    target_link_libraries(Zafkiel PUBLIC GL)
endif()
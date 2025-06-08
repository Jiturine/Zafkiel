
file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c )

add_executable(Sandbox ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(Sandbox PUBLIC Zafkiel)
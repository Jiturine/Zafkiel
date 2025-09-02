
file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c )

add_executable(Sandbox ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(Sandbox PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Sandbox
)

target_link_libraries(Sandbox PUBLIC Zafkiel)

add_subdirectory(assets/scripts)

add_dependencies(Sandbox AppAssembly)
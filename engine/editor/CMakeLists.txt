file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp src/*.tpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c )

add_executable(Editor ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Editor
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Editor
)


target_include_directories(Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Editor PUBLIC Zafkiel imgui)
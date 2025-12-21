include(${CMAKE_SOURCE_DIR}/3rdparty/catch2/extras/Catch.cmake)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(engine_tests ${SOURCE_FILES})

target_link_libraries(engine_tests PRIVATE Catch2::Catch2WithMain engine)

catch_discover_tests(engine_tests)
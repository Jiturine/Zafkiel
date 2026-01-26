cmake_minimum_required(VERSION 3.12)

project(Zafkiel)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(3rdparty)

add_subdirectory(parser)

add_subdirectory(engine)

add_subdirectory(editor)

enable_testing()

add_subdirectory(engine_tests)

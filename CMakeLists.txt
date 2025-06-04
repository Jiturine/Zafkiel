cmake_minimum_required(VERSION 3.5)
project(Zafkiel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdparty)

add_subdirectory(engine)

add_subdirectory(sandbox)
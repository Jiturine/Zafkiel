include(../test_macros.cmake)

add_executable(world world.cpp)
mark_as_cli_test(world)
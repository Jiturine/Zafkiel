SET(SANDBOX_NAME Sandbox)

SET(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${SANDBOX_NAME} main.cpp)

target_link_libraries(${SANDBOX_NAME} PUBLIC Zafkiel)
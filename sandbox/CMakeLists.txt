SET(SANDBOX_NAME Sandbox)

add_executable(${SANDBOX_NAME} main.cpp)

target_link_libraries(${SANDBOX_NAME} PUBLIC Zafkiel)
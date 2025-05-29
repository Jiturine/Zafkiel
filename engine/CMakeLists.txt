set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp" "*.tpp")
file(GLOB_RECURSE SOURCE_FILES "*.cpp" "*.c")

add_library(Zafkiel SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(Zafkiel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries(Zafkiel PUBLIC spdlog)

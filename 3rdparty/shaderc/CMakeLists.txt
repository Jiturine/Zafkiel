file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/libshaderc_util/src/*.cc)

add_library(shaderc_util STATIC ${SOURCE_FILES})

target_include_directories(shaderc_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libshaderc_util/include)

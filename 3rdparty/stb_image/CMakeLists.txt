add_library(stb_image STATIC stb_image_impl.cpp stb_image_write_impl.cpp)

target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
if (NOT TARGET spdlog)
    add_subdirectory(spdlog)
endif()

if (NOT TARGET Catch2)
    add_subdirectory(catch2)
endif()

if (NOT TARGET mono)
    add_subdirectory(mono)
endif()

if (NOT TARGET EnTT)
    add_subdirectory(entt)
endif()

if (NOT TARGET imgui)
    add_library(imgui STATIC 
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backends/imgui_impl_sdl3.cpp
        imgui/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC
        SDL/include
        imgui
    )

endif()

if (NOT TARGET SDL3::SDL3)
    add_subdirectory(SDL)
endif()

if (NOT TARGET glad)
    add_subdirectory(glad)
endif()

if (NOT TARGET glm)
    add_subdirectory(glm)
endif()

if (NOT TARGET yaml-cpp)
    add_subdirectory(yaml-cpp)
endif()

if (NOT stb_image)
    add_subdirectory(stb_image)
endif()
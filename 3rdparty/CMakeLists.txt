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
    add_subdirectory(imgui)
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
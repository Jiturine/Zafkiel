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
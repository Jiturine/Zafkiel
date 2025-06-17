if (NOT TARGET spdlog)
    add_subdirectory(spdlog)
endif()

if (NOT TARGET Catch2)
    add_subdirectory(catch2)
endif()
macro(mark_as_cli_test target_name)
    add_test(NAME ${target_name}
        COMMAND ${target_name}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    target_link_libraries(${target_name} PRIVATE Catch2WithMain Zafkiel)
endmacro()
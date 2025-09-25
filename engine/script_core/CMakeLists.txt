add_custom_target(
    script_core
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND csc -target:library 
                -debug:portable 
                -out:${CMAKE_BINARY_DIR}/bin/ScriptCore.dll 
                *.cs
    VERBATIM
)

add_custom_command(TARGET script_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/bin/ScriptCore.pdb
        ${CMAKE_BINARY_DIR}/bin/editor
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/bin/ScriptCore.dll
        ${CMAKE_BINARY_DIR}/bin/editor
    VERBATIM
)
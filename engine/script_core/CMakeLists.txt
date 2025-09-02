
add_custom_target(
    ScriptCore
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND mcs -target:library -out:${CMAKE_BINARY_DIR}/bin/ScriptCore.dll *.cs
)

add_custom_command(TARGET ScriptCore POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/bin/ScriptCore.dll
        ${CMAKE_BINARY_DIR}/bin/Sandbox
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/bin/ScriptCore.dll
        ${CMAKE_BINARY_DIR}/bin/Editor
    VERBATIM
)

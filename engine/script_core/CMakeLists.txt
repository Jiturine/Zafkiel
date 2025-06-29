
add_custom_target(
    ScriptCore
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND mcs -target:library -out:${CMAKE_BINARY_DIR}/bin/ScriptCore.dll *.cs
)
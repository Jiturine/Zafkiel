
add_custom_target(
    AppAssembly
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND mcs -reference:${CMAKE_BINARY_DIR}/bin/ScriptCore.dll -target:library -out:${CMAKE_BINARY_DIR}/bin/Sandbox/AppAssembly.dll *.cs
)

# add_custom_command(TARGET AppAssembly POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_BINARY_DIR}/bin/AppAssembly.dll
#         ${CMAKE_BINARY_DIR}/bin/Sandbox
#     VERBATIM
# )



add_custom_target(
    AppAssembly
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND mcs -reference:${CMAKE_BINARY_DIR}/bin/ScriptCore.dll -target:library -out:${CMAKE_BINARY_DIR}/bin/AppAssembly.dll *.cs
)
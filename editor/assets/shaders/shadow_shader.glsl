#type vertex
#version 450 core

#include "schema/shadow_pass.zss"
#include "schema/mesh_object.zss"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

void main()
{
    gl_Position = uShadow.lightViewProjection * uMeshObject.modelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

void main()
{
}
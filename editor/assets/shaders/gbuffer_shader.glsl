#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

#include "schema/global.zss"

#include "schema/mesh_object.zss"

layout(location = 0) out vec3 v_FragPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) flat out uint v_EntityID;

void main()
{
    gl_Position = uGlobal.viewProjectionMatrix * uMeshObject.modelMatrix * vec4(a_Position, 1.0);
    v_FragPos = vec4(uMeshObject.modelMatrix * vec4(a_Position, 1.0)).xyz;
    mat3 normalMatrix = transpose(inverse(mat3(uMeshObject.modelMatrix)));
    v_Normal = normalize(normalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    v_EntityID = uMeshObject.entityID;
}

#type fragment
#version 450 core
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out uint gEntityID;

layout(location = 0) in vec3 v_FragPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) flat in uint v_EntityID;

#include "schema/blinn_phong.zss"

void main()
{
    gPosition = vec4(v_FragPos, 1.0);
    gNormal = vec4(normalize(v_Normal), 1.0);
    gAlbedo = vec4(texture(uDiffuseTexture, v_TexCoord).rgb, 1.0);
    gEntityID = v_EntityID;
}
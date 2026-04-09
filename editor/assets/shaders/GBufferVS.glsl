#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

static uniform GlobalUBO 
{
    vec3 viewPos;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
} uGlobal;

static uniform MeshObjectUBO 
{
    mat4 modelMatrix;
    uint entityID;
} uMeshObject;

layout(location = 0) out vec3 v_FragPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) flat out uint v_EntityID;
layout(location = 4) out vec3 v_Tangent;
layout(location = 5) out vec3 v_Bitangent;

void main()
{
    gl_Position = uGlobal.viewProjectionMatrix * uMeshObject.modelMatrix * vec4(a_Position, 1.0);
    v_FragPos = vec4(uMeshObject.modelMatrix * vec4(a_Position, 1.0)).xyz;

    mat3 normalMatrix = transpose(inverse(mat3(uMeshObject.modelMatrix)));
    v_Normal = normalize(normalMatrix * a_Normal);
    v_Tangent = normalize(normalMatrix * a_Tangent.xyz);
    v_Bitangent = normalize(normalMatrix * (cross(a_Normal, a_Tangent.xyz) * a_Tangent.w));

    v_TexCoord = a_TexCoord;
    v_EntityID = uMeshObject.entityID;
}


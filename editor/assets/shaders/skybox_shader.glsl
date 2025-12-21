#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;

layout(binding = 0) uniform UBO {
    mat4 viewProjection;
} ubo;

layout(location = 0) out vec3 v_TexCoord;

void main()
{
    vec4 pos = ubo.viewProjection * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
    v_TexCoord = a_Position;
}

#type fragment
#version 450 core
layout(location = 0) in vec3 v_TexCoord;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint EntityID;

layout(binding = 1) uniform UBO {
    samplerCube skybox;
    uint nullEntity;
} ubo;

void main()
{
    FragColor = texture(ubo.skybox, v_TexCoord);
    EntityID = ubo.nullEntity;
}
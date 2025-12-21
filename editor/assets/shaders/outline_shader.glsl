#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 viewProjection;
} ubo;

void main()
{
    float outlineThickness = 0.02f;
    vec3 expandedPos = a_Position + a_Normal * outlineThickness;
    gl_Position = ubo.viewProjection * ubo.model * vec4(expandedPos, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0);
}
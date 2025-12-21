#type vertex
#version 450 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform UBO {
    sampler2D screenTexture;
} ubo;

layout(location = 0) in vec2 v_TexCoord;

void main()
{
    vec4 linearColor = texture(ubo.screenTexture, v_TexCoord);
    FragColor = vec4(pow(linearColor.xyz, vec3(1.0 / 2.2)), linearColor.w);
}
#version 450 core

uniform sampler2D uScreenTexture;

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 v_TexCoord;

void main()
{
    FragColor = texture(uScreenTexture, v_TexCoord);
}
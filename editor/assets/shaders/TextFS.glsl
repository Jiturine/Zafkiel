#version 450 core
layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uFontAtlasTexture;

void main()
{
    vec4 texColor = texture(uFontAtlasTexture, v_TexCoord);
    FragColor = texColor * v_Color;
}



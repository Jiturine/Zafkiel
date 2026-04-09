#version 450 core
layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) flat in int v_TexIndex;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform sampler2D uTexture3;
uniform sampler2D uTexture4;
uniform sampler2D uTexture5;
uniform sampler2D uTexture6;
uniform sampler2D uTexture7;
uniform sampler2D uTexture8;
uniform sampler2D uTexture9;
uniform sampler2D uTexture10;
uniform sampler2D uTexture11;
uniform sampler2D uTexture12;
uniform sampler2D uTexture13;
uniform sampler2D uTexture14;
uniform sampler2D uTexture15;

void main()
{
    vec4 texColor;

    switch(v_TexIndex)
    {
        case 0: texColor = texture(uTexture0, v_TexCoord); break;
        case 1: texColor = texture(uTexture1, v_TexCoord); break;
        case 2: texColor = texture(uTexture2, v_TexCoord); break;
        case 3: texColor = texture(uTexture3, v_TexCoord); break;
        case 4: texColor = texture(uTexture4, v_TexCoord); break;
        case 5: texColor = texture(uTexture5, v_TexCoord); break;
        case 6: texColor = texture(uTexture6, v_TexCoord); break;
        case 7: texColor = texture(uTexture7, v_TexCoord); break;
        case 8: texColor = texture(uTexture8, v_TexCoord); break;
        case 9: texColor = texture(uTexture9, v_TexCoord); break;
        case 10: texColor = texture(uTexture10, v_TexCoord); break;
        case 11: texColor = texture(uTexture11, v_TexCoord); break;
        case 12: texColor = texture(uTexture12, v_TexCoord); break;
        case 13: texColor = texture(uTexture13, v_TexCoord); break;
        case 14: texColor = texture(uTexture14, v_TexCoord); break;
        case 15: texColor = texture(uTexture15, v_TexCoord); break;
        default: texColor = vec4(1.0, 1.0, 1.0, 1.0); break;
    }

    FragColor = texColor * v_Color;
}

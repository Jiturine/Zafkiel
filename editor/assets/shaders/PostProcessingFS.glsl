#version 450 core

uniform sampler2D uScreenTexture;

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 v_TexCoord;

void main()
{
    vec3 hdrColor = texture(uScreenTexture, v_TexCoord).rgb;

    // tone mapping
    vec3 linearColor = hdrColor / (hdrColor + vec3(1.0));

    // gamma correction
    FragColor = vec4(pow(linearColor, vec3(1.0 / 2.2)), 1);
}

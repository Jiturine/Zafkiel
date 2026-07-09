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

uniform TextUBO
{
    float pxRange;
} uText; // set to distance field's pixel range

vec2 sqr(vec2 x) { return x * x; } // squares vector components

float screenPxRange() 
{
    vec2 msdfTextureSize;
    switch(v_TexIndex)
    {
        case 0: msdfTextureSize = vec2(textureSize(uTexture0, 0)); break;
        case 1: msdfTextureSize = vec2(textureSize(uTexture1, 0)); break;
        case 2: msdfTextureSize = vec2(textureSize(uTexture2, 0)); break;
        case 3: msdfTextureSize = vec2(textureSize(uTexture3, 0)); break;
        case 4: msdfTextureSize = vec2(textureSize(uTexture4, 0)); break;
        case 5: msdfTextureSize = vec2(textureSize(uTexture5, 0)); break;
        case 6: msdfTextureSize = vec2(textureSize(uTexture6, 0)); break;
        case 7: msdfTextureSize = vec2(textureSize(uTexture7, 0)); break;
        default: msdfTextureSize = vec2(1.0, 1.0); break;
    }
    vec2 unitRange = vec2(uText.pxRange) / msdfTextureSize;
    // If inversesqrt is not available, use vec2(1.0)/sqrt
    vec2 screenTexSize = inversesqrt(sqr(dFdx(v_TexCoord)) + sqr(dFdy(v_TexCoord)));
    // Can also be approximated as screenTexSize = vec2(1.0)/fwidth(v_TexCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) 
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3 msd;
    switch(v_TexIndex)
    {
        case 0: msd = texture(uTexture0, v_TexCoord).rgb; break;
        case 1: msd = texture(uTexture1, v_TexCoord).rgb; break;
        case 2: msd = texture(uTexture2, v_TexCoord).rgb; break;
        case 3: msd = texture(uTexture3, v_TexCoord).rgb; break;
        case 4: msd = texture(uTexture4, v_TexCoord).rgb; break;
        case 5: msd = texture(uTexture5, v_TexCoord).rgb; break;
        case 6: msd = texture(uTexture6, v_TexCoord).rgb; break;
        case 7: msd = texture(uTexture7, v_TexCoord).rgb; break;
        default: msd = vec3(1.0, 1.0, 1.0); break;
    }

    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    FragColor = vec4(v_Color.rgb, opacity);
}
#version 450 core
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out float gMetalness;
layout(location = 4) out float gRoughness;
layout(location = 5) out uint gEntityID;

layout(location = 0) in vec3 v_FragPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) flat in uint v_EntityID;
layout(location = 4) in vec3 v_Tangent;
layout(location = 5) in vec3 v_Bitangent;

uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uMetalnessTexture;
uniform sampler2D uRoughnessTexture;

void main()
{
    gPosition = vec4(v_FragPos, 1.0);

    vec3 normalWS;
    vec3 sampledNormal = texture(uNormalTexture, v_TexCoord).rgb;

    if (length(sampledNormal - vec3(0.5, 0.5, 1.0)) < 0.01)
    {
        normalWS = normalize(v_Normal);
    }
    else
    {
        vec3 normalTS = sampledNormal * 2.0 - 1.0;
        mat3 TBN = mat3(normalize(v_Tangent), normalize(v_Bitangent), normalize(v_Normal));
        normalWS = normalize(TBN * normalTS);
    }
    gNormal = vec4(normalWS, 1.0);
    gAlbedo = vec4(texture(uDiffuseTexture, v_TexCoord).rgb, 1.0);
    gMetalness = texture(uMetalnessTexture, v_TexCoord).r;
    gRoughness = texture(uRoughnessTexture, v_TexCoord).r;
    gEntityID = v_EntityID;
}
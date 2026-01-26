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

layout(location = 0) in vec2 v_TexCoord;

struct DirLight
{
    vec3 color;
    float intensity;
    vec3 direction;
};

#include "schema/global.zss"

#include "schema/shading_pass.zss"

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), 32);

    vec3 ambient = 0.2 * light.intensity * light.color * albedo;
    vec3 diffuse = diff * light.intensity * light.color * albedo;
    vec3 specular = spec * light.intensity * light.color * vec3(1.0);
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 FragPos = texture(uPosition, v_TexCoord).rgb;
    vec3 Normal = texture(uNormal, v_TexCoord).rgb;
    vec3 Albedo = texture(uAlbedo, v_TexCoord).rgb;
    DirLight testLight;
    testLight.color = vec3(1.0, 1.0, 1.0);
    testLight.intensity = 1.0;
    testLight.direction = vec3(-1.0, 0.0, 0.0);

    vec3 viewDir = normalize(uGlobal.viewPos - FragPos);
    FragColor = vec4(CalcDirLight(testLight, Normal, viewDir, Albedo), 1.0);
}
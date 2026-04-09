#version 450 core
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 v_TexCoord;

struct DirLight
{
    vec3 color;
    float intensity;
    vec3 direction;
};

static uniform GlobalUBO 
{
    vec3 viewPos;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
} uGlobal;

uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedo;
uniform sampler2D uMetalness;
uniform sampler2D uRoughness;
uniform sampler2D uShadowMap;

uniform ShadowUBO 
{
    mat4 lightViewProjection;
} uShadow;

float CalcShadow(vec3 fragPos)
{
    vec4 lightSpacePos = uShadow.lightViewProjection * vec4(fragPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w; 
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), 32);

    vec3 ambient = 0.2 * light.intensity * light.color * albedo;
    vec3 diffuse = diff * light.intensity * light.color * albedo;
    vec3 specular = spec * light.intensity * light.color * vec3(1.0);
    float shadow = CalcShadow(fragPos);
    return ambient + (diffuse + specular) * (1 - shadow);
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
    FragColor = vec4(CalcDirLight(testLight, FragPos, Normal, viewDir, Albedo), 1.0);
}
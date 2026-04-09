#version 450 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 v_TexCoord;

struct DirLight
{
    vec3 color;
    float intensity;
    vec3 direction;
};

static uniform GlobalUBO {
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

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    const float PI = 3.14159265359;
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 CalcDirLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metalness, float roughness)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfDir = normalize(lightDir + viewDir);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metalness);
    vec3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

    float NDF = DistributionGGX(normal, halfDir, roughness);       
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);       
    vec3 f_specular = NDF * G * F / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001);

    const float PI = 3.14159265359;
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 f_diffuse = (1.0 - metalness) * (vec3(1.0) - F) * albedo / PI;

    vec3 result = (f_diffuse + f_specular) * light.color * light.intensity * NdotL;
    return result;
}

void main()
{
    vec3 FragPos = texture(uPosition, v_TexCoord).rgb;
    vec3 Normal = texture(uNormal, v_TexCoord).rgb;
    vec3 Albedo = texture(uAlbedo, v_TexCoord).rgb;
    float metalness = texture(uMetalness, v_TexCoord).r;
    float roughness = texture(uRoughness, v_TexCoord).r;
    DirLight testLight;
    testLight.color = vec3(1.0, 1.0, 1.0);
    testLight.intensity = 5.0;
    testLight.direction = vec3(-1.0, 0.0, 0.0);

    vec3 viewDir = normalize(uGlobal.viewPos - FragPos);

    FragColor = vec4(CalcDirLight(testLight, FragPos, Normal, viewDir, Albedo, metalness, roughness), 1.0);
}
#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 viewProjection;
    uint entityID;
} ubo;

layout(location = 0) out vec3 v_FragPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) flat out uint v_EntityID;

void main()
{
    gl_Position = ubo.viewProjection * ubo.model * vec4(a_Position, 1.0);
    v_FragPos = vec4(ubo.model * vec4(a_Position, 1.0)).xyz;
    mat3 normalMatrix = transpose(inverse(mat3(ubo.model)));
    v_Normal = normalize(normalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    v_EntityID = ubo.entityID;
}

#type fragment
#version 450 core
layout(location = 0) in vec3 v_FragPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) flat in uint v_EntityID;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint EntityID;

struct DirLight
{
    vec3 color;
    float intensity;
    vec3 direction;
};
layout(binding = 1) uniform UBO {
    DirLight dirLight;
    vec3 viewPos;
} ubo;
layout(binding = 2) uniform sampler2D diffuseTex;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoord)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), 32);

    vec3 ambient = 0.2 * light.intensity * light.color * vec3(texture(diffuseTex, texCoord));
    vec3 diffuse = diff * light.intensity * light.color * vec3(texture(diffuseTex, texCoord));
    vec3 specular = spec * light.intensity * light.color * vec3(1.0);
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 viewDir = normalize(ubo.viewPos - v_FragPos);
    FragColor = vec4(CalcDirLight(ubo.dirLight, v_Normal, viewDir, v_TexCoord), 1.0);

    EntityID = v_EntityID;
}
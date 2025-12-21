#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_TexIndex;
layout(location = 4) in uint a_EntityID;

layout(binding = 0) uniform UBO {
    mat4 viewProjection;
} ubo;

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) flat out int v_TexIndex;
layout(locaiton = 3) flat out uint v_EntityID;

void main()
{
    gl_Position = ubo.viewProjection * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_EntityID = a_EntityID;
}

#type fragment
#version 450 core
layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) flat in int v_TexIndex;
layout(locaiton = 3) flat in uint v_EntityID;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint EntityID;

layout(binding = 1) uniform UBO {
    sampler2D textures[32];
} ubo;

void main()
{
    vec4 texColor = texture(ubo.textures[v_TexIndex], v_TexCoord);
    FragColor = texColor * v_Color;
    EntityID = v_EntityID;
}
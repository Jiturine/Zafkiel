#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform uint u_EntityID;

out vec3 v_Normal;
out vec2 v_TexCoord;
flat out uint v_EntityID;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    v_Normal = normalize(normalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    v_EntityID = u_EntityID;
}

#type fragment
#version 330 core
in vec3 v_Normal;
in vec2 v_TexCoord;
flat in uint v_EntityID;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint EntityID;

void main()
{
    vec3 normalColor = vec3(v_Normal + 1) * 0.5;
    FragColor = vec4(normalColor, 1.0);
    EntityID = v_EntityID;
}
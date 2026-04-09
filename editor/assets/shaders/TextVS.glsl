#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in uint a_EntityID;

uniform UIQuadUBO
{
    mat4 viewProjection;
} uText;

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
    gl_Position = utext.viewProjection * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
}
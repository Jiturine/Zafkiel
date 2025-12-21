#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;

layout(set = 0, binding = 0) uniform UBO {
    vec3 a_Offset;
} ubo;

void main()
{
    gl_Position = vec4(a_Position + ubo.a_Offset, 1.0);
}
#type fragment
#version 450 core
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
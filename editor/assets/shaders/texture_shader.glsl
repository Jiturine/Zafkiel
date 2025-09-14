#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_TexIndex;
uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
flat out int v_TexIndex;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
}

#type fragment
#version 330 core
in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_TexIndex;

out vec4 FragColor;

uniform sampler2D u_Textures[32];

void main()
{
    vec4 texColor = texture(u_Textures[v_TexIndex], v_TexCoord);
    FragColor = texColor * v_Color;
}
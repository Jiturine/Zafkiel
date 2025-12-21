#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

out VS_OUT
{
    vec3 normal;
}
vs_out;

uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_View * u_Model * vec4(a_Position, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));
    vs_out.normal = normalize(normalMatrix * a_Normal);
}

#type geometry
#version 450 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec3 normal;
}
gs_in[];

const float MAGNITUDE = 0.4;

uniform mat4 u_Projection;

void GenerateLine(int index)
{
    gl_Position = u_Projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}

#type fragment
#version 330 core

in vec3 v_Normal;
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.8, 0.0, 1.0);
}

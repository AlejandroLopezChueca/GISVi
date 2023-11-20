#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Offset;
layout(location = 2) in mat4 a_Transform;

layout(std140, binding = 0) uniform u_ViewProjection
{
	mat4 viewProjection;
};

void main()
{
    gl_Position = viewProjection * a_Transform * vec4(a_Pos + a_Offset, 1.0);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
}

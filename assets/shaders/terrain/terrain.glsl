#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;

uniform mat4 u_Transform;

layout(std140, binding = 0) uniform u_ViewProjection
{
	mat4 viewProjection;
};

out float height;

void main()
{
    // This height is with repect the mean value
    height = a_Pos.y;
    gl_Position =  viewProjection * u_Transform * vec4(a_Pos, 1.0f);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in float height;

uniform vec3 u_MaxMinMeanHeight;

layout(binding = 0) uniform sampler1D u_ColorMap;

void main()
{
    //float h =  height/(u_MaxMinHeight.x);
    float h = (height + u_MaxMinMeanHeight.z - u_MaxMinMeanHeight.y)/(u_MaxMinMeanHeight.x - u_MaxMinMeanHeight.y);
    color = texture(u_ColorMap, max(h,0));
    //color = vec4(h, h, h, 1.0f);
}

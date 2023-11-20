#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

layout(std140, binding = 0) uniform u_ViewProjection
{
	mat4 viewProjection;
};

layout(std140, binding = 1) uniform u_ClipPlaneData
{
	vec4 clipDistance[8];
};

//uniform vec4 u_ClipPlane[8];
//uniform int  u_ClipPlanesUsed;

out float height;

void main()
{
    // This height is with repect the mean value
    height = a_Pos.y;
    gl_Position =  viewProjection * u_Transform * vec4(a_Pos, 1.0f);
    for (int idx = 0; idx < 8; idx++)
    {
    	//if (idx >= u_ClipPlanesUsed) {break;}
    	gl_ClipDistance[idx] = dot(vec4(a_Pos, 1.0f), clipDistance[idx]);
    }
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

#version 330 core

layout(location = 0)	in vec2 coord3d;
layout(location = 1)	in vec2 vertexUV;

out vec2 UV;

void main()
{
    vec2 vertexPosition = coord3d - vec2(400,300);
    vertexPosition /= vec2(400,300);
    gl_Position =  vec4(vertexPosition,0,1);
    UV = vertexUV;
}
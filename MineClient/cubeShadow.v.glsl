#version 330 core

layout(location = 0)	in vec3 coord3d;

uniform mat4 depthVP;
uniform mat4 Model;
 
void main()
{
	gl_Position = depthVP * Model * vec4(coord3d,1);
}
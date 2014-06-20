#version 330 core

in vec3 World;

out vec3 color;
 
uniform sampler2D myTextureSampler;
uniform float myTime;

void main()
{
	vec2 UV = vec2(0, 0);
 	color = texture( myTextureSampler, UV ).rgb * 0.0001 * myTime + vec3(1, 0, 1);
}
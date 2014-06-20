#version 330 core
 
in vec2 UV;

out vec4 color;
 
uniform sampler2D myTextureSampler;

void main()
{
	color = texture( myTextureSampler, UV );
	if (color.b < 0.01 && color.r < 0.01 && color.g < 0.01)
		color.a = 0;
}
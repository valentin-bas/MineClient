#version 330 core

layout(location = 0)	in vec3 coord3d;

						out vec2 UV;

						uniform int side;
 
void main()
{
	gl_Position = vec4(coord3d, 1);
    UV = (coord3d.xy + vec2(1, 1)) / 2.0;
	if (side > 0)
    	UV.x = UV.x / 2.0 + 0.5;
    else
    	UV.x /= 2.0;
}
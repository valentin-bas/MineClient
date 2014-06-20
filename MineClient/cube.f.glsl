#version 330 core
 
in vec2 UV;
in vec3 Normal;
in vec3 World;

out vec3 color;
 
uniform sampler2D myTextureSampler;
uniform float myTime;
 
void main()
{
	float	T = myTime / 12000.0f;
	float	ratioDay = (T - floor(T));
	float	Ttmp = ratioDay / 0.8f;
	float	angleLight = Ttmp * 3.141592;
	float	dayNight = clamp(10.0 * ratioDay, 0, 1);
	if (ratioDay > 0.7f)
		dayNight = clamp(-10.0 * ratioDay + 8, 0, 1);
	vec3	toLight;//normalize(light);
	toLight.x = cos(angleLight);
	toLight.y = sin(angleLight);
	toLight.z = 0.3;
	toLight = normalize(toLight);
	vec3	lightColor = vec3(1.0, 1.0, 1.0);
	float	lightInt = dayNight * clamp(dot(Normal, toLight), 0, 1);
 
	color = clamp(texture( myTextureSampler, UV ).rgb * lightColor * lightInt + texture( myTextureSampler, UV ).rgb * 0.2, 0, 1);
}
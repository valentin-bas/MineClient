#version 330 core
 
in vec2 UV;

out vec3 color;
 
uniform sampler2D myTextureSampler;

struct occulus
{
	vec2 LensCenter;
	vec2 ScreenCenter;
	vec2 Scale;
	vec2 ScaleIn;
	vec4 HmdWarpParam;	
};

uniform occulus occulusBuffer;

vec2 HmdWarp(vec2 in01)
{
	vec2 theta = (in01 - occulusBuffer.LensCenter) * occulusBuffer.ScaleIn; // Scales to [-1, 1]
	float rSq = theta.x * theta.x + theta.y * theta.y;
	vec2 rvector= theta * (occulusBuffer.HmdWarpParam.x + occulusBuffer.HmdWarpParam.y * rSq +
							 occulusBuffer.HmdWarpParam.z * rSq * rSq +
							 occulusBuffer.HmdWarpParam.w * rSq * rSq * rSq);
	return occulusBuffer.LensCenter + occulusBuffer.Scale * rvector;
}

void main()
{
	color = vec3(0, 0, 0);
	vec2 tc = HmdWarp(UV);
 	if (!all(equal(clamp(tc, occulusBuffer.ScreenCenter-vec2(0.25,0.5), occulusBuffer.ScreenCenter+vec2(0.25,0.5)), tc)))
 		return;
 	color = texture( myTextureSampler, tc ).rgb;
}
#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 coord3d;
layout(location = 2) in vec3 vertexNormal;
layout(location = 1) in vec2 vertexUV;
 
// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Normal;
out vec3 World;
//out vec3 ViewDir;
 
// Values that stay constant for the whole mesh.
//uniform vec3 CamPos;
uniform mat4 ViewProj;
uniform mat4 Model;
 
void main()
{
    gl_Position = ViewProj *  Model * vec4(coord3d, 1);
    World = (Model * vec4(coord3d, 1)).rgb;
    UV = vertexUV;
    Normal = vertexNormal;
    
    //ViewDir = normalize(CamPos - World);
}
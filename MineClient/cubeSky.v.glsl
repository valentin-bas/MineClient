#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 coord3d;
 
// Output data ; will be interpolated for each fragment.
out vec3 World;
 
// Values that stay constant for the whole mesh.
uniform mat4 ViewProj;
uniform mat4 Model;
 
void main()
{
    gl_Position = ViewProj *  Model * vec4(coord3d, 1);
    World = (Model * vec4(coord3d, 1)).rgb;
}
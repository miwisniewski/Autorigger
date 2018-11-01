#version 330

layout (location = 0) in vec3 Position; 
layout (location = 1) in vec2 TexCoord; 
layout (location = 2) in vec3 Normal; 

out vec2 TexCoord0;

uniform mat4 matrix;

void main() 
{
	TexCoord0 = TexCoord;
	gl_Position = matrix * vec4(Position, 1.0);
}
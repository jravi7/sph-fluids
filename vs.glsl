#version 330

in vec3 position; 
in vec2 texcoord; 
in vec3 normal;
in vec3 color;

uniform mat4 projection;

out vec2 texcoord0;
out vec3 normal0 ;
out vec4 color0;

void main()
{
	gl_Position = projection * vec4(position, 1.0f);
	texcoord0 = texcoord; 
	normal0 = (projection * vec4(normal, 0.0f)).xyz; 
	color0 = vec4(color, 1.0);
}
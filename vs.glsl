#version 330

in vec3 position; 
in vec2 texcoord; 
in vec3 normal;
in vec4 color;

uniform mat4 projection;

out vec2 texcoord0;
out vec3 normal0 ;
out vec4 f_color;

void main()
{
	gl_Position = projection * vec4(position, 1.0f);
	texcoord0 = texcoord; 
	normal0 = (projection * vec4(normal, 0.0f)).xyz; 
	f_color = color;
}
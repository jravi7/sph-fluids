#version 330

in vec4 color0;
in vec3 normal0; 
in vec2 texcoord0; 

out vec4 outputColor;
uniform sampler2D gtexture;

void main()
{			
		outputColor = vec4(1,1,1,1);
}

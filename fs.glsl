#version 330

in vec4 color0;
in vec3 normal0; 
in vec2 texcoord0; 



out vec4 outputColor;
uniform sampler2D gtexture;

void main()
{		
		//vec4 dyncolor = vec4(gl_FragCoord.y, gl_FragCoord.x, gl_FragCoord.z, 1.0);
		//outputColor = vec4(1.f, 1.f, 1.f, 1.f);
		//outputColor = color0;
		outputColor = texture2D(gtexture, texcoord0);
}

#version 330

in vec4 color0;
in vec3 normal0; 
in vec2 texcoord0; 



out vec4 outputColor;
uniform sampler2D gtexture;

void main()
{		
		float ratio = gl_FragCoord.y / 500.0f;
		//vec4 dyncolor = mix(vec4(gl_FragCoord.y/100, gl_FragCoord.x/100, gl_FragCoord.z, 1.0/100), color0, ratio);
		vec4 dyncolor = vec4(gl_FragCoord.x/500, gl_FragCoord.y/500, gl_FragCoord.z/100, 1.0);
		//outputColor = color0;
		outputColor = dyncolor;
}

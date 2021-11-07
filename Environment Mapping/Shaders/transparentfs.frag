#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture2;

void main()
{
	vec4 TexColor = texture(texture2,TexCoords);
	//if(TexColor.a < 0.1f)
		//discard;
	FragColor=TexColor;
	//FragColor = vec4(vec3(ZView(gl_FragCoord.z)),1.0f);
}
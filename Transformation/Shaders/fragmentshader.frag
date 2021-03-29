#version 450 core

in vec3 VertexColor;
in vec2 TexCoordinate;

uniform sampler2D outtexture1;
uniform sampler2D outtexture2;

uniform float mixvalue;

out vec4 FragColor;
void main()
{
	FragColor = mix(texture(outtexture1,TexCoordinate),texture(outtexture2,TexCoordinate),mixvalue);
}

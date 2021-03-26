#version 450 core

out vec4 FragColor;
in vec3 vertexColor;
in vec2 texCoordinate;

// texture samplers
uniform sampler2D outtexture1;
uniform sampler2D outtexture2;
//»ìºÏ±ÈÀý
uniform float mixvalue;
void main()
{
	// linearly interpolate between both textures
	//FragColor=mix(texture(outtexture1,texCoordinate),texture(outtexture2,texCoordinate),0.5);
	FragColor=mix(texture(outtexture1,texCoordinate), texture(outtexture2,vec2(1-texCoordinate.x, texCoordinate.y)),mixvalue);
	
}
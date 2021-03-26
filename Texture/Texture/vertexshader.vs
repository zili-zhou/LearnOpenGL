#version 450 core

layout(location=0) in vec3 Pos;
layout(location=1) in vec3 Color;
layout(location=2) in vec2 Texture;

out vec3 vertexColor;
out vec2 texCoordinate;

void main()
{
	gl_Position=vec4(Pos,1.0f);
    vertexColor=Color; 
	texCoordinate=vec2(Texture.x,Texture.y);
}
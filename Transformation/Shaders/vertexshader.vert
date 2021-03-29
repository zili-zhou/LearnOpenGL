#version 450 core

layout(location=0) in vec3 Pos;
layout(location=1) in vec3 Color;
layout(location=2) in vec2 Texture;

out vec3 VertexColor;
out vec2 TexCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position=projection*view*model*vec4(Pos,1.0f);
	VertexColor=Color;
	TexCoordinate=vec2(Texture.x,1.0f-Texture.y);
}


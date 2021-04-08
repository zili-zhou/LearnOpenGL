#version 450 core

layout(location=0) in vec3 Pos;
layout(location=1) in vec3 Color;
layout(location=2) in vec2 Texture;
layout(location=3) in vec3 Normal;

out vec3 VertexColor;
out vec2 TexCoordinate;
out vec3 VertexNormal;
out vec3 FragPos;//ÿ���������������ϵλ��,���ڼ��������

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection * view * model * vec4(Pos,1.0f);
	VertexColor=Color;
	TexCoordinate=vec2(Texture.x,1.0f-Texture.y);

	VertexNormal=mat3(transpose(inverse(model)))*Normal;//����Ҳͨ��model����ת����������ϵ��transpose(M^-1)

	FragPos = vec3(model * vec4(Pos,1.0f));
}


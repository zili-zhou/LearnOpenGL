#version 450 core
layout (location=0) in vec3 Pos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = Pos;
	vec4 pos = projection*view*vec4(Pos,1.0f);
	gl_Position = pos.xyww;
}

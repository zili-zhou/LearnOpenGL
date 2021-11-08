#version 450 core
layout(location=0) in vec3 Pos;
/*
layout(location=1) in vec3 Color;
layout(location=2) in vec2 TextureCoords;
*/
layout(location=1) in vec3 NormalL;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = vec3(transpose(inverse(model)) * vec4(NormalL, 1.0f));
    Position = vec3(model * vec4(Pos, 1.0f));
    gl_Position = projection * view * model * vec4(Pos, 1.0f);
}
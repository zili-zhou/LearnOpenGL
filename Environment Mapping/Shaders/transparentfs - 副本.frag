#version 450 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 CameraPos;
uniform samplerCube skybox;

void main()
{
	float ratio = 1.0f/1.52f;
	vec3 View = normalize(Position - CameraPos);
	vec3 InputLight_reflect = reflect(View , normalize(Normal));//∑¥…‰reflect(View , normalize(Normal));
	vec3 InputLight_refract = refract(View, normalize(Normal), ratio);//’€…‰
	FragColor = vec4(texture(skybox, InputLight_refract).rgb, 1.0f);
}
#version 450 core
out vec4 FragColor;

in vec3 VertexColor;
in vec2 TexCoordinate;
in vec3 VertexNormal;
in vec3 FragPos;

uniform sampler2D texture1;

float near = 0.01f;
float far = 100.0f;

float ZView(float depth)
{
	float z_view = 2*far*near/((far + near)+depth*(near-far));
	return z_view;
}

void main()
{
	FragColor=texture(texture1,TexCoordinate);
	//FragColor = vec4(vec3(ZView(gl_FragCoord.z)),1.0f);
}
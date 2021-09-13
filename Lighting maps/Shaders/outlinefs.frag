#version 450 core
out vec4 FragColor;

void main()
{
	FragColor=vec4(0.04, 0.28, 0.26, 1.0);
	//FragColor = vec4(vec3(ZView(gl_FragCoord.z)),1.0f);
}
#include<glad/glad.h>
#include<GLFW/glfw3.h>


const char* VertexShaderSource = "#version 450 core\n"
"layout(location=0) in vec3 Pos;\n"
"void main()\n "
"{\n"
"	gl_Position=vec4(Pos.x,Pos.y,Pos.z,1.0);\n"
"}\n";

int main()
{
	//build and compile our shader program
	//vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//�������ɫ��Դ�븽�ӵ���ɫ�������ϣ�Ȼ�������
	glShaderSource(vertexShader, 1,&VertexShaderSource 0);


	float vertices[] = { -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.0f,0.5f,0.0f };

	//ʹ��glGenBuffers������һ������ID����һ��VBO����
	unsigned int VertexBufferObj;
	glGenBuffers(1, &VertexBufferObj);
	//���㻺�����Ļ���������GL_ARRAY_BUFFER,ʹ��glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ��
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObj);
	//֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	return 0;
}
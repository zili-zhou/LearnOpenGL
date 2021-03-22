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
	//把这个着色器源码附加到着色器对象上，然后编译它
	glShaderSource(vertexShader, 1,&VertexShaderSource 0);


	float vertices[] = { -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.0f,0.5f,0.0f };

	//使用glGenBuffers函数和一个缓冲ID生成一个VBO对象
	unsigned int VertexBufferObj;
	glGenBuffers(1, &VertexBufferObj);
	//顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER,使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObj);
	//之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	return 0;
}
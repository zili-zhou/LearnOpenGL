#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<shader.h>

const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;


void FrameSizeBufferCallback(GLFWwindow* window, int width, int height);

void InputProcess(GLFWwindow* window);

int main()
{
	//glfw create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Triangle", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameSizeBufferCallback);//传入回调函数

	//glad load all OpenGL function pointers
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	Shader shader("vertexshader.vs", "fragmentshader.fs");


	//顶点信息
	float vertices[] = {
		// 位置              // 颜色
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
	};
	unsigned int indices[] = { 0,1,2};

	//使用glGenBuffers函数和一个缓冲ID生成一个VBO/VAO对象,索引缓冲对象 Element Buffer Object
	unsigned int VBO,VAO,EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER,使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	//链接顶点属性,VAO绑定VertexArray后，所有属性都在VAO里,第一个参数是顶点属性的位置，最后两个分别是一个顶点的步长stride,该顶点特性的偏移量offset 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//以顶点属性位置值作为参数，启用顶点属性
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//解绑VBO,VAO
	glBindBuffer(GL_ARRAY_BUFFER,0);
	//glBindVertexArray(0);
	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		/*
		//激活这个程序对象,在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个程序对象
		glUseProgram(ShaderProgram);
		
		float timeValue = glfwGetTime();
		float colorValue = std::sin(timeValue) / 2.0f + 0.5f;
		int UniformLocation = glGetUniformLocation(ShaderProgram, "vertexColor");
		glUniform4f(UniformLocation,  colorValue, 0.0f,0.0f, 1.0f);//调用Uniform变量之前需先激活Shader Program
		*/
		shader.use();

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,0);//绘制图元的类型,绘制顶点数，索引类型，EBO偏移量

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	glfwTerminate();
	return 0;
}

void FrameSizeBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
}

void InputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}
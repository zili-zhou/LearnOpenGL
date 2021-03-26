#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<shaders/shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;

float mixvalue = 0.2f;

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
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	unsigned int indices[] = { 0,3,1,
							   1,3,2};

	//使用glGenBuffers函数和一个缓冲ID生成一个VBO/VAO对象,索引缓冲对象 Element Buffer Object
	unsigned int VBO, VAO, EBO;
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//以顶点属性位置值作为参数，启用顶点属性
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//解绑VBO,VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//加载并生成纹理1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//设置纹理对象的环绕和采样方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//加载图片
	int Width, Height, ColorChanne;//加载好图片后，这三个变量分别保存图片的宽，高和颜色通道
	stbi_set_flip_vertically_on_load(true);
	const char* filename = "..//textures//container.jpg";
	unsigned char* data = stbi_load(filename, &Width, &Height, &ColorChanne, 0);

	if (data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load texture1" << std::endl;
	}

	stbi_image_free(data);

	//加载并生成纹理2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	//设置纹理对象的环绕和采样方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//加载图片
	filename = "..//textures//awesomeface.png";
	data = stbi_load(filename, &Width, &Height, &ColorChanne, 0);

	if (data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load texture2" << std::endl;
	}

	stbi_image_free(data);
	//使用Uniform变量之前需激活shader program
	shader.use();
	//设置uniform变量
	shader.SetInt("outtexture1", 0);
	shader.SetInt("outtexture2", 1);
	shader.SetFloat("mixvalue", mixvalue);
	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// 在绑定纹理之前先激活纹理单元
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
	
		shader.SetFloat("mixvalue", mixvalue);
		shader.use();

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//绘制图元的类型,绘制顶点数，索引类型，EBO偏移量


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
	//鼠标上下键控制混合比例
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixvalue += mixvalue <= 1.0f ? 0.001f : 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixvalue -= mixvalue >= 0.0f ? 0.001f : 0.0f;
	}
}


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
	glfwSetFramebufferSizeCallback(window, FrameSizeBufferCallback);//����ص�����

	//glad load all OpenGL function pointers

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shader("vertexshader.vs", "fragmentshader.fs");

	
	//������Ϣ
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	unsigned int indices[] = { 0,3,1,
							   1,3,2};

	//ʹ��glGenBuffers������һ������ID����һ��VBO/VAO����,����������� Element Buffer Object
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//���㻺�����Ļ���������GL_ARRAY_BUFFER,ʹ��glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	//���Ӷ�������,VAO��VertexArray���������Զ���VAO��,��һ�������Ƕ������Ե�λ�ã���������ֱ���һ������Ĳ���stride,�ö������Ե�ƫ����offset 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//�Զ�������λ��ֵ��Ϊ���������ö�������
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//���VBO,VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//���ز���������1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//�����������Ļ��ƺͲ�����ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//����ͼƬ
	int Width, Height, ColorChanne;//���غ�ͼƬ�������������ֱ𱣴�ͼƬ�Ŀ��ߺ���ɫͨ��
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

	//���ز���������2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	//�����������Ļ��ƺͲ�����ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ
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
	//ʹ��Uniform����֮ǰ�輤��shader program
	shader.use();
	//����uniform����
	shader.SetInt("outtexture1", 0);
	shader.SetInt("outtexture2", 1);
	shader.SetFloat("mixvalue", mixvalue);
	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// �ڰ�����֮ǰ�ȼ�������Ԫ
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
	
		shader.SetFloat("mixvalue", mixvalue);
		shader.use();

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//����ͼԪ������,���ƶ��������������ͣ�EBOƫ����


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
	//������¼����ƻ�ϱ���
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixvalue += mixvalue <= 1.0f ? 0.001f : 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixvalue -= mixvalue >= 0.0f ? 0.001f : 0.0f;
	}
}


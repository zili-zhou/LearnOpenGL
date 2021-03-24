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
		// λ��              // ��ɫ
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ����
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ����
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ����
	};
	unsigned int indices[] = { 0,1,2};

	//ʹ��glGenBuffers������һ������ID����һ��VBO/VAO����,����������� Element Buffer Object
	unsigned int VBO,VAO,EBO;
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//�Զ�������λ��ֵ��Ϊ���������ö�������
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//���VBO,VAO
	glBindBuffer(GL_ARRAY_BUFFER,0);
	//glBindVertexArray(0);
	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		/*
		//��������������,��glUseProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������
		glUseProgram(ShaderProgram);
		
		float timeValue = glfwGetTime();
		float colorValue = std::sin(timeValue) / 2.0f + 0.5f;
		int UniformLocation = glGetUniformLocation(ShaderProgram, "vertexColor");
		glUniform4f(UniformLocation,  colorValue, 0.0f,0.0f, 1.0f);//����Uniform����֮ǰ���ȼ���Shader Program
		*/
		shader.use();

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,0);//����ͼԪ������,���ƶ��������������ͣ�EBOƫ����

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
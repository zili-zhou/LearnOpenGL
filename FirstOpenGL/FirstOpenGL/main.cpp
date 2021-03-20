#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;

//���û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ����������ǿ��ԶԴ���ע��һ���ص�����(Callback Function)��������ÿ�δ��ڴ�С��������ʱ�򱻵���
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
//һ�������Լ�һ��������Ϊ���롣����������᷵����������Ƿ����ڱ�����
void InputProcess(GLFWwindow* window);

int main()
{
	// glfw initialize and configure
	glfwInit();
	//"Major" and "minor" OpenGL�汾���������֣�OpenGL4.5 major��4��minor��5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//GLFW_OPENGL_PROFILEָ��ҪΪ�䴴�������ĵ�OpenGL�����ļ�
	 
	//glfw greate a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGLWIN", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "failed to create a window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);//����ص�����

	//glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

//�ص���������ÿ֡���ӿ�
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void InputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}
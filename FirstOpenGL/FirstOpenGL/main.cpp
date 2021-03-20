#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;

//当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数(Callback Function)，它会在每次窗口大小被调整的时候被调用
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
//一个窗口以及一个按键作为输入。这个函数将会返回这个按键是否正在被按下
void InputProcess(GLFWwindow* window);

int main()
{
	// glfw initialize and configure
	glfwInit();
	//"Major" and "minor" OpenGL版本的两个数字，OpenGL4.5 major是4，minor是5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//GLFW_OPENGL_PROFILE指定要为其创建上下文的OpenGL配置文件
	 
	//glfw greate a window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGLWIN", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "failed to create a window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);//传入回调函数

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

//回调函数设置每帧的视口
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void InputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>


const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;


const char* VertexShaderSource = "#version 450 core\n"
"layout(location=0) in vec3 Pos;\n"
"void main()\n "
"{\n"
"	gl_Position=vec4(Pos.x,Pos.y,Pos.z,1.0);\n"
"}\0";

const char* FragmentShaderSource = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n "
"{\n"
"	FragColor=vec4(1.0f,1.0f,0.0f,1.0f);\n"
"}\0";

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
		//glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//build and compile our shader program
	//vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//把这个着色器源码附加到着色器对象上，然后编译它
	glShaderSource(vertexShader, 1,&VertexShaderSource,NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//链接着色器程序ShaderProgram
	unsigned int ShaderProgram;
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);
	// check for linking errors
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
 
	//把着色器对象链接到程序对象以后，记得删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);




	//顶点信息
	float vertices[] = { -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.0f,0.5f,0.0f };//left, right, top

	//使用glGenBuffers函数和一个缓冲ID生成一个VBO/VAO对象
	unsigned int VBO,VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);

	//顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER,使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//链接顶点属性,VAO绑定VertexArray后，所有属性都在VAO里
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//以顶点属性位置值作为参数，启用顶点属性
	glEnableVertexAttribArray(0);



	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//激活这个程序对象,在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个程序对象
		glUseProgram(ShaderProgram);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);//绘制图元的类型,顶点数组的起始索引,绘制多少个顶点

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
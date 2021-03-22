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
	glfwSetFramebufferSizeCallback(window, FrameSizeBufferCallback);//����ص�����

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
	//�������ɫ��Դ�븽�ӵ���ɫ�������ϣ�Ȼ�������
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

	//������ɫ������ShaderProgram
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
 
	//����ɫ���������ӵ���������Ժ󣬼ǵ�ɾ����ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);




	//������Ϣ
	float vertices[] = { -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.0f,0.5f,0.0f };//left, right, top

	//ʹ��glGenBuffers������һ������ID����һ��VBO/VAO����
	unsigned int VBO,VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);

	//���㻺�����Ļ���������GL_ARRAY_BUFFER,ʹ��glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//���Ӷ�������,VAO��VertexArray���������Զ���VAO��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//�Զ�������λ��ֵ��Ϊ���������ö�������
	glEnableVertexAttribArray(0);



	//render loop
	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//��������������,��glUseProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������
		glUseProgram(ShaderProgram);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);//����ͼԪ������,�����������ʼ����,���ƶ��ٸ�����

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
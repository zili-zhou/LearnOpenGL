#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<shader_h/shader.h>
#include<camera_h/camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<iostream>
#include<cmath>
#include<vector>
#include<algorithm>

const unsigned int WIN_WIDTH = 1440;
const unsigned int WIN_HEIGHT = 1080;

float mixvalue = 0.2f;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

float deltaTime = 0.0f;//两帧之差
float lastFrame = 0.0f;//上一帧的时间

void FrameSizeBufferCallback(GLFWwindow* window, int width, int height);
void MouseCursorCallback(GLFWwindow* window, double pos_x, double  pos_y);//鼠标光标回调函数
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);//鼠标滚轮回调函数,x/yoffset:scroll offset along the x/y-axis.
void InputProcess(GLFWwindow* window);

bool firstMouse = true;
double lastX = WIN_WIDTH / 2.0f;//鼠标上一帧位于屏幕的位置，初始化于屏幕中心
double lastY = WIN_HEIGHT / 2.0f;

glm::vec3 LightPos(12.0f, 10.0f, 2.0f);

int main()
{
	//create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Transformation", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create Window." << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameSizeBufferCallback);
	glfwSetCursorPosCallback(window, MouseCursorCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	//捕获鼠标光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glad load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}



	//顶点信息
	glEnable(GL_DEPTH_TEST);//允许深度测试

	float vertices[] = {
		// positions          // colors           // texture coords  //normals
		//前面后面
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,	0.0f,0.0f,1.0f,	      // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,	0.0f,0.0f,1.0f,		    // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,	0.0f,0.0f,1.0f,			// bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,	0.0f,0.0f,1.0f,			// top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,	0.0f,0.0f,-1.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,	0.0f,0.0f,-1.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,	0.0f,0.0f,-1.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   0.0f, 1.0f,	0.0f,0.0f,-1.0f,
		//上面和下面
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,	0.0f,1.0f,0.0f,	// top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,	0.0f,-1.0f,0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,	0.0f,-1.0f,0.0f,	// bottom left
		- 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,	0.0f,1.0f,0.0f,	// top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,	0.0f,1.0f,0.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,	0.0f,-1.0f,0.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,	0.0f,-1.0f,0.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f,	0.0f,1.0f,0.0f,

		//左边和右面
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,	1.0f,0.0f,0.0f,	// top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,	1.0f,0.0f,0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,	-1.0f,0.0f,0.0f,	// bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,	-1.0f,0.0f,0.0f,	// top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   -1.0f,0.0f,0.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,	-1.0f,0.0f,0.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f,	1.0f,0.0f,0.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f,	1.0f,0.0f,0.0f,
		/*
		//前面后面
		// positions          // colors           // texture coords //normals
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,		0.0f,0.0f,1.0f,      // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		0.0f,0.0f,1.0f,	    // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,		0.0f,0.0f,1.0f,		// bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,		0.0f,0.0f,1.0f,		// top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,		0.0f,0.0f,-1.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		0.0f,0.0f,-1.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,		0.0f,0.0f,-1.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   0.0f, 1.0f,		0.0f,0.0f,-1.0f,
		//上面和下面
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,		0.0f,1.0f,0.0f,// top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		0.0f,-1.0f,0.0f,// bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,		0.0f,-1.0f,-0.0f// bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,		0.0f,1.0f,1.0f,// top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,		0.0f,1.0f,0.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,		0.0f,-1.0f,0.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,		0.0f,-1.0f,0.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f,		0.0f,1.0f,0.0f,
		
		//左边和右面
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,		1.0f,0.0f,0.0f,// top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		1.0f,0.0f,0.0f,// bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,		-1.0f,0.0f,-0.0f// bottom left
		- 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,		-1.0f,0.0f,1.0f,// top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,		-1.0f,0.0f,0.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,	    -1.0f,0.0f,0.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,		1.0f,0.0f,0.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f,		1.0f,0.0f,0.0f,
		*/
	};

	unsigned int indices[] = { 0,3,1,
							1,3,2,
							4,6,5,
							4,7,6,

							15,11,8,
							12,11,15,
							10,13,9,
							13,14,9,
		
							23,16,22,
							22,16,17,
							20,19,18,
							20,21,18};

	//位移向量
	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(-2.0f,  5.0f, -0.0f),
	  glm::vec3(-1.5f, 2.2f, -2.5f),
	  glm::vec3(-3.8f, 2.0f, -2.3f),
	  glm::vec3(2.4f, -0.4f, -1.5f),
	  glm::vec3(-1.7f,  3.0f, -0.5f),
	  glm::vec3(1.3f, -2.0f, -2.5f),
	  glm::vec3(1.5f,  4.0f, -0.1f),
	  glm::vec3(3.5f,  0.2f, -1.5f),
	  glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//立方体光源八个顶点
	float light[] =
	{
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,

		-0.5f,0.5f,-1.0f,
		-0.5f,-0.5f,-1.0f,
		0.5f,-0.5f,-1.0f,
		0.5f,0.5f,-1.0f,

		 0.5f,  0.5f, 0.0f,   
		 0.5f, -0.5f, 0.0f,   
		-0.5f, -0.5f, 0.0f,   
		-0.5f,  0.5f, 0.0f,   

		-0.5f,0.5f,-1.0f,	  
		-0.5f,-0.5f,-1.0f,    
		0.5f,-0.5f,-1.0f,     
		0.5f,0.5f,-1.0f,
	};

	unsigned int indicesLight[] = { 
							0,3,1,
							1,3,2,
							7,0,6,
							6,0,1,
							4,6,5,
							4,7,6,
							4,3,2,
							4,5,2,
							15,11,8,
							12,11,15,
							10,13,9,
							13,14,9 };

	//vertex buffer object, vertex array object, element buffer object
	unsigned int CubeVBO, CubeVAO, CubeEBO;
	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &CubeVBO);
	glGenBuffers(1, &CubeEBO);

	glBindVertexArray(CubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(3, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	


	//vertex buffer light, vertex array light, element buffer light
	unsigned int LightVAO, LightVBO, LightEBO;
	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);

	glGenBuffers(1, &LightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, LightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(light), &light, GL_STATIC_DRAW);

	glGenBuffers(1, &LightEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LightEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLight), &indicesLight, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);




	//加载并生成纹理
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//stbi_set_flip_vertically_on_load(true);
	int Width, Height, ColorChannel;
	const char* filename = "..//textures//container.jpg";
	unsigned char* data = stbi_load(filename, &Width, &Height, &ColorChannel, 0);

	if (data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load Texture." << std::endl;
	}
	stbi_image_free(data);


	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	filename = "..//textures//awesomeface.png";
	data = stbi_load(filename, &Width, &Height, &ColorChannel, 0);

	if (data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load Texture." << std::endl;
	}
	stbi_image_free(data);

	//cueb shader
	Shader CubeShader("..//Shaders//vertexshader.vert", "..//Shaders//fragmentshader.frag");
	CubeShader.use();
	CubeShader.SetInt("outtexture1", 0);
	CubeShader.SetInt("outtexture2", 1);
	CubeShader.SetFloat("mixvalue", mixvalue);
	CubeShader.SetVec3("Light.LightPos", LightPos);


	//light shader
	Shader LightShader("..//Shaders//lightvs.vert", "..//Shaders//lightfs.frag");
	LightShader.use();
	//light shader



	while (!glfwWindowShouldClose(window))
	{
		double currTime = glfwGetTime();
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;

		InputProcess(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//每帧清空颜色缓存和深度缓存

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//render the cube
		CubeShader.use();
		CubeShader.SetFloat("mixvalue", mixvalue);
		CubeShader.SetVec3("CameraPos", camera.GetPosition());

		//设置mvp矩阵并传入着色器
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = camera.GetViewMatrix();
		CubeShader.SetMat4("view", view);

		projection = glm::perspective(glm::radians(camera.m_Fov), static_cast<float>(WIN_WIDTH / WIN_HEIGHT), 0.001f, 10000.0f);
		CubeShader.SetMat4("projection", projection);

		glBindVertexArray(CubeVAO);
		for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
		{

			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			CubeShader.SetMat4("model", model);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		//光特性
		//glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 LightColor = glm::vec3(1.0f);
		LightColor.x = std::sin(glfwGetTime() * 0.5f);
		LightColor.x = std::sin(glfwGetTime() * 0.5f);
		LightColor.x = std::sin(glfwGetTime() * 1.0f);
		glm::vec3 diffuseColor = LightColor * 0.7f;
		glm::vec3 ambientColor = diffuseColor * 0.4f;
		CubeShader.SetVec3("Light.ambient", ambientColor);
		CubeShader.SetVec3("Light.diffuse", diffuseColor);
		CubeShader.SetVec3("Light.specular", 1.0f, 1.0f, 1.0f);

		//材质特性
		CubeShader.SetVec3("Material.ambient", 1.0f, 1.0f, 1.0f);
		CubeShader.SetVec3("Material.diffuse", 1.0f, 1.0f, 1.0f);
		CubeShader.SetVec3("Material.specular", 1.0f, 1.0f, 1.0f);

		//render the light
		LightShader.use();
		LightShader.SetMat4("view", view);
		LightShader.SetMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, LightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		LightShader.SetMat4("model", model);
		glBindVertexArray(LightVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &CubeVBO);
	glDeleteBuffers(1, &CubeEBO);
	glDeleteVertexArrays(1, &CubeVAO);

	glDeleteBuffers(1, &LightVBO);
	glDeleteBuffers(1, &LightEBO);
	glDeleteVertexArrays(1, &LightVAO);

	glfwTerminate();
	return 0;
}


void FrameSizeBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void MouseCursorCallback(GLFWwindow* window, double pos_x, double  pos_y)
{
	if (firstMouse)//鼠标第一次进入屏幕时就更新lastX/Y
	{
		lastX = pos_x;
		lastY = pos_y;
		firstMouse = false;
	}

	//计算x/y方向的位移并更新lastX/Y
	double offsetX = pos_x - lastX;
	double offsetY = lastY - pos_y;//y方向上需要反转，因为鼠标向上，希望场景也向上，此时摄像机就要向下
	lastX = pos_x;
	lastY = pos_y;

	camera.ProcessMouseCursor(offsetX, offsetY, true);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void InputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);



	float CameraSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		deltaTime *= 3.0f;
	}
	//float CameraSpeed = 0.005f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movements::FORWARDS, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movements::BACKWARDS, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movements::LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movements::RIGHT, deltaTime);
	}
}


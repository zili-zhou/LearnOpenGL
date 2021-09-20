#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<shader_h/shader.h>
#include<camera_h/camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include<algorithm>

const unsigned int WIN_WIDTH = 1440;
const unsigned int WIN_HEIGHT = 1080;


Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

float deltaTime = 0.0f;//两帧之差
float lastFrame = 0.0f;//上一帧的时间

void FrameSizeBufferCallback(GLFWwindow* window, int width, int height);
void MouseCursorCallback(GLFWwindow* window, double pos_x, double  pos_y);//鼠标光标回调函数
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);//鼠标滚轮回调函数,x/yoffset:scroll offset along the x/y-axis.
void InputProcess(GLFWwindow* window);

unsigned int LoadTexture(const char* path);//加载纹理地址，返回纹理的ID

bool firstMouse = true;
double lastX = WIN_WIDTH / 2.0f;//鼠标上一帧位于屏幕的位置，初始化于屏幕中心
double lastY = WIN_HEIGHT / 2.0f;

glm::vec3 LightPos(10.0f, 8.0f,5.0f);

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

	
	glEnable(GL_DEPTH_TEST);//允许深度测试
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	//顶点信息
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
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,	0.0f,1.0f,0.0f,	// top left 

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
							20,21,18 };

	//位移向量
	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(-2.0f,  0.5f, 10.0f),
	  glm::vec3(3.5f, 0.0f, 4.5f),
	  glm::vec3(-3.8f, 2.0f, 2.3f),
	  glm::vec3(-2.4f, 0.4f, 1.5f),
	  glm::vec3(-1.7f,  3.0f, -0.5f),
	  glm::vec3(-1.3f, 2.0f, 5.5f),
	  glm::vec3(-1.5f,  4.0f, 6.1f),
	  glm::vec3(-3.5f,  0.2f, 5.5f),
	  glm::vec3(-1.3f,  1.0f, 7.5f)
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


	float planevertices[] = {
		// positions          // colors           // texture coords  //normals
		//前面后面
		15.0f,  -0.5f, 20.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,	0.0f,0.0f,1.0f,	      // top right
		 15.0f, -0.55f, 20.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,	0.0f,0.0f,1.0f,		    // bottom right
		-15.0f, -0.55f, 20.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,	0.0f,0.0f,1.0f,			// bottom left
		-15.0f,  -0.5f, 20.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,	0.0f,0.0f,1.0f,			// top left 

		-15.0f,-0.5f,-5.0f,	  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,	0.0f,0.0f,-1.0f,
		-15.0f,-0.55f,-5.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,	0.0f,0.0f,-1.0f,
		15.0f,-0.55f,-5.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,	0.0f,0.0f,-1.0f,
		15.0f,-0.5f,-5.0f,      1.0f, 1.0f, 0.0f,   0.0f, 1.0f,	0.0f,0.0f,-1.0f,
		//上面和下面
		// positions          // colors           // texture coords
		 15.0f,  -0.5f, 20.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,	0.0f,1.0f,0.0f,	// top right
		 15.0f, -0.55f, 20.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,	0.0f,-1.0f,0.0f,	// bottom right
		-15.0f, -0.55f, 20.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,	0.0f,-1.0f,0.0f,	// bottom left
		-15.0f,  -0.5f, 20.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,	0.0f,1.0f,0.0f,	// top left 

		-15.0f,-0.5f,-5.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,	0.0f,1.0f,0.0f,
		-15.0f,-0.55f,-5.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,	0.0f,-1.0f,0.0f,
		15.0f,-0.55f,-5.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,	0.0f,-1.0f,0.0f,
		15.0f,-0.5f,-5.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f,	0.0f,1.0f,0.0f,

		//左边和右面
		15.0f,  -0.5f, 20.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,	1.0f,0.0f,0.0f,	// top right
		 15.0f, -0.55f, 20.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,	1.0f,0.0f,0.0f,	// bottom right
		-15.0f, -0.55f, 20.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,	-1.0f,0.0f,0.0f,	// bottom left
		-15.0f,  -0.5f, 20.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,	-1.0f,0.0f,0.0f,	// top left 

		-15.0f,-0.5f,-5.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   -1.0f,0.0f,0.0f,
		-15.0f,-0.55f,-5.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,	-1.0f,0.0f,0.0f,
		15.0f,-0.55f,-5.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f,	1.0f,0.0f,0.0f,
		15.0f,-0.5f,-5.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f,	1.0f,0.0f,0.0f,

	};

	unsigned int planeindices[] = { 0,3,1,
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
							20,21,18 };


	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(1.0f, 0.0f, -0.6f)
	};

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


	// plane VAO
	unsigned int planeVAO, planeVBO, planeEBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planevertices), &planevertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeindices), &planeindices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, false, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);


	//transparent objects
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);

	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//加载并生成纹理
	unsigned int diffuseMap = LoadTexture("..//textures//container2.png");
	unsigned int specularMap = LoadTexture("..//textures//container2_specular.png");
	unsigned int floorTexture = LoadTexture("..//textures//toy_box_diffuse.png");
	unsigned int transparentTexture = LoadTexture("..//textures//window.png");

	//cueb shader
	Shader CubeShader("..//Shaders//vertexshader.vert", "..//Shaders//fragmentshader.frag");
	CubeShader.use();
	CubeShader.SetInt("Material.diffuse", 0);
	CubeShader.SetInt("Material.specular", 1);

	CubeShader.SetVec3("Light.LightPos", LightPos);


	//light shader
	Shader LightShader("..//Shaders//lightvs.vert", "..//Shaders//lightfs.frag");
	LightShader.use();

	//plane shader
	Shader PlaneShader("..//Shaders//vertexshader.vert", "..//Shaders//planefs.frag");
	//PlaneShader.use();
	//PlaneShader.SetInt("texture1", 2);

	//transparent shader
	Shader TransparentShader("..//Shaders//transparentvs.vert", "..//Shaders//transparentfs.frag");
	TransparentShader.use();
	TransparentShader.SetInt("texture2",3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window))
	{
		double currTime = glfwGetTime();
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;

		InputProcess(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//每帧清空颜色缓存和深度缓存

		//设置mvp矩阵并传入着色器
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.m_Fov), static_cast<float>(WIN_WIDTH / WIN_HEIGHT), 0.1f, 100.0f);

		TransparentShader.use();
		TransparentShader.SetMat4("view", view);
		TransparentShader.SetMat4("projection", projection);
		

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
		
		// floor
		PlaneShader.use();	
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		PlaneShader.SetMat4("model", glm::mat4(1.0f));
		PlaneShader.SetMat4("view", view);
		PlaneShader.SetMat4("projection", projection);
		PlaneShader.SetInt("texture1", 2);
		glBindVertexArray(planeVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);

		//render the cube
		CubeShader.use();
		CubeShader.SetVec3("CameraPos", camera.GetPosition());
		CubeShader.SetMat4("view", view);
		CubeShader.SetMat4("projection", projection);

		//光特性
		CubeShader.SetVec3("Light.ambient", 0.5f,0.5f,0.5f);
		CubeShader.SetVec3("Light.diffuse", 0.5f, 0.5f, 0.5f);
		CubeShader.SetVec3("Light.specular", 1.0f, 1.0f, 1.0f);

		//材质特性
		CubeShader.SetFloat("Material.shininess", 64.0f);		

		glBindVertexArray(CubeVAO);
		for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
		{
				
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			//float angle = 10.0f * i;
			//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			CubeShader.SetMat4("model", model);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		
		//grass
		//先绘制所有不透明的物体。
		//对所有半透明的物体排序。
		//按由远及近的顺序绘制所有半透明的物体。
		
		//半透明物体距离摄像机从小到大排序
		std::map<float, glm::vec3> sortedMap;

		for (unsigned int i = 0; i < vegetation.size(); ++i)
		{
			float distance = glm::length(camera.GetPosition() - vegetation[i]);
			sortedMap[distance] = vegetation[i];
		}
		
		TransparentShader.use();
		glBindVertexArray(transparentVAO);

		//逆向渲染
		for (auto iterator = sortedMap.rbegin(); iterator != sortedMap.rend(); ++iterator)
		{		
			float angle = 90.0f;
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, transparentTexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model,iterator->second);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			TransparentShader.SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &CubeVBO);
	glDeleteBuffers(1, &CubeEBO);
	glDeleteVertexArrays(1, &CubeVAO);

	glDeleteBuffers(1, &LightVBO);
	glDeleteBuffers(1, &LightEBO);
	glDeleteVertexArrays(1, &LightVAO);

	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &planeEBO);
	glDeleteVertexArrays(1, &planeVAO);

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


unsigned int LoadTexture(const char* path)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);



	int Width, Height, ColorChannel;
	unsigned char* data = stbi_load(path, &Width, &Height, &ColorChannel, 0);

	if (data != nullptr)
	{
		GLenum format;
		if (ColorChannel == 1)
			format = GL_RED;
		else if (ColorChannel == 3)
			format = GL_RGB;
		else if (ColorChannel == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);	
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE :  GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		std::cerr << "Failed to load Texture." << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

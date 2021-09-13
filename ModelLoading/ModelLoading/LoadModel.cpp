
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<shader_h/shader.h>
#include<camera_h/camera.h>
#include<model_h/model.h>

#include<filesystem.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<iostream>
#include<cmath>
#include<vector>
#include<algorithm>

const unsigned int WIN_WIDTH = 1440;
const unsigned int WIN_HEIGHT = 1080;


Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

float deltaTime = 0.0f;//两帧之差
float lastFrame = 0.0f;//上一帧的时间

void FrameSizeBufferCallback(GLFWwindow* window, int width, int height);
void MouseCursorCallback(GLFWwindow* window, double pos_x, double  pos_y);//鼠标光标回调函数
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);//鼠标滚轮回调函数,x/yoffset:scroll offset along the x/y-axis.
void InputProcess(GLFWwindow* window, Shader shader);

unsigned int LoadTexture(const char* path);//加载纹理地址，返回纹理的ID

bool firstMouse = true;
double lastX = WIN_WIDTH / 2.0f;//鼠标上一帧位于屏幕的位置，初始化于屏幕中心
double lastY = WIN_HEIGHT / 2.0f;

glm::vec3 LightPos(4.0f, 5.0f, 2.0f);


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

	stbi_set_flip_vertically_on_load(true);

	//顶点信息
	

	// 四个点光源位置
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
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


	Shader shader("..//Shaders//vertexshader.vert", "..//Shaders//fragmentshader.frag");

	Model Load_model(FileSystem::getPath("resources//objects//nanosuit//nanosuit.obj"));

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


	//light shader
	Shader LightShader("..//Shaders//lightvs.vert", "..//Shaders//lightfs.frag");
	LightShader.use();



	while (!glfwWindowShouldClose(window))
	{
		double currTime = glfwGetTime();
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;

		InputProcess(window, shader);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//每帧清空颜色缓存和深度缓存


		//render the cube
		shader.use();
		shader.SetVec3("CameraPos", camera.GetPosition());

		//设置mvp矩阵并传入着色器
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = camera.GetViewMatrix();
		shader.SetMat4("view", view);

		projection = glm::perspective(glm::radians(camera.m_Fov), static_cast<float>(WIN_WIDTH / WIN_HEIGHT), 0.001f, 10000.0f);
		shader.SetMat4("projection", projection);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader.SetMat4("model", model);

		//光特性

		// directional light
		shader.SetVec3("DirLight.LightDir", -0.2f, -1.0f, -0.3f);
		shader.SetVec3("DirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("DirLight.diffuse", 0.5f, 0.5f, 0.5f);
		shader.SetVec3("DirLight.specular", 0.5f, 0.5f, 0.5f);

		// point light 1
		shader.SetVec3("PointLights[0].LightPos", pointLightPositions[0]);
		shader.SetVec3("PointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("PointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.SetVec3("PointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.SetFloat("PointLights[0].constant", 1.0f);
		shader.SetFloat("PointLights[0].linear", 0.09);
		shader.SetFloat("PointLights[0].quadratic", 0.032);

		// point light 2
		shader.SetVec3("PointLights[1].LightPos", pointLightPositions[1]);
		shader.SetVec3("PointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("PointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader.SetVec3("PointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.SetFloat("PointLights[1].constant", 1.0f);
		shader.SetFloat("PointLights[1].linear", 0.09);
		shader.SetFloat("PointLights[1].quadratic", 0.032);

		// point light 3
		shader.SetVec3("PointLights[2].LightPos", pointLightPositions[2]);
		shader.SetVec3("PointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("PointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shader.SetVec3("PointLights[2].specular", 1.0f, 1.0f, 1.0f);
		shader.SetFloat("PointLights[2].constant", 1.0f);
		shader.SetFloat("PointLights[2].linear", 0.09);
		shader.SetFloat("PointLights[2].quadratic", 0.032);

		// point light 4
		shader.SetVec3("PointLights[3].LightPos", pointLightPositions[3]);
		shader.SetVec3("PointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("PointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shader.SetVec3("PointLights[3].specular", 1.0f, 1.0f, 1.0f);
		shader.SetFloat("PointLights[3].constant", 1.0f);
		shader.SetFloat("PointLights[3].linear", 0.09);
		shader.SetFloat("PointLights[3].quadratic", 0.032);



		// spotLight
		shader.SetVec3("SpotLight.LightPos", camera.GetPosition());
		shader.SetVec3("SpotLight.LightDir", camera.GetFront());
		shader.SetVec3("SpotLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("SpotLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.SetVec3("SpotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.SetFloat("SpotLight.constant", 1.0f);
		shader.SetFloat("SpotLight.linear", 0.09);
		shader.SetFloat("SpotLight.quadratic", 0.032);
		shader.SetFloat("SpotLight.innerAngle", glm::cos(glm::radians(12.5f)));
		shader.SetFloat("SpotLight.outterAngle", glm::cos(glm::radians(15.0f)));

		Load_model.Draw(shader);

		//render the light
		LightShader.use();
		LightShader.SetMat4("view", view);
		LightShader.SetMat4("projection", projection);
		glBindVertexArray(LightVAO);
		for (int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f));
			LightShader.SetMat4("model", model);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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

void InputProcess(GLFWwindow* window, Shader shader)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);


	//移动方位
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

	shader.use();
	//调整光源
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		shader.SetBool("IsDirectionLight", true);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		shader.SetBool("IsPointLight", true);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		shader.SetBool("IsSpotLight", true);
	}
}


/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem.h>
#include <shader_h/shader.h>
#include <camera_h/camera.h>
#include <model_h/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window,Shader shader);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("..//Shaders//lightvs.vert", "..//Shaders//lightfs.frag");

	// load models
	// -----------
	Model ourModel(FileSystem::getPath("resources/objects/planet/planet.obj"));
	//Model Model("../resources/objects/cyborg/cyborg.obj");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window,ourShader);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.SetMat4("projection", projection);
		ourShader.SetMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.SetMat4("model", model);
		ourModel.Draw(ourShader);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, Shader shader)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);


	//移动方位
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

	shader.use();
	//调整光源
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		shader.SetBool("IsDirectionLight", true);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		shader.SetBool("IsPointLight", true);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		shader.SetBool("IsSpotLight", true);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseCursor(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


*/
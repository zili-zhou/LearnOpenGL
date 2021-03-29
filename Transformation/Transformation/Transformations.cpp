#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<shader_h/shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<iostream>

#include<vector>

const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;

float mixvalue = 0.2f;
float moveleftright = 0.0f;
float moveupdown = 0.0f;
bool IsLeftRight=false;

void FrameSizeBufferCallback(GLFWwindow* window, int width, int height);

void InputProcess(GLFWwindow* window);

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


	//glad load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}



	//顶点信息
	glEnable(GL_DEPTH_TEST);//允许深度测试

	float vertices []= {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		//上面和下面
				// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // top left 

		-0.5f,0.5f,-1.0f,	  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f,-0.5f,-1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		0.5f,-0.5f,-1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,0.5f,-1.0f,      1.0f, 1.0f, 0.0f,   1.0f, 1.0f	

		

	};

	unsigned int indices []={ 0,3,1,
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
							13,14,9}; 

	//位移向量
	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  -0.2f),
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

	//vertex buffer object, vertex array object, element buffer object
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE,data);
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

	//shader
	Shader shader("..//Shaders//vertexshader.vert", "..//Shaders//fragmentshader.frag");
	shader.use();
	shader.SetInt("outtexture1", 0);
	shader.SetInt("outtexture2", 1);
	shader.SetFloat("mixvalue", mixvalue);

	while (!glfwWindowShouldClose(window))
	{
		InputProcess(window);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//每帧清空颜色缓存和深度缓存

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		shader.use();
		
		shader.SetFloat("mixvalue", mixvalue);
		/*
		//先缩放再旋转
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));
		transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
		
		//变换矩阵传递给着色器
		unsigned int transformMatrix = glGetUniformLocation(shader.m_ProgramID, "transform");
		glUniformMatrix4fv(transformMatrix, 1, GL_FALSE,&transform[0][0]);
		*/
		//设置mvp矩阵并传入着色器
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f); 

		if(IsLeftRight==true)//左右旋转
			model = glm::rotate(model, moveleftright *glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
		model = glm::rotate(model, moveupdown * glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(80.0f), static_cast<float>(WIN_WIDTH / WIN_HEIGHT), 0.001f, 10000.0f);

		unsigned int ModelLocation = glGetUniformLocation(shader.m_ProgramID, "model");
		glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
		unsigned int ViewLocation = glGetUniformLocation(shader.m_ProgramID, "view");
		glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, &view[0][0]);
		unsigned int PerspectiveLocation = glGetUniformLocation(shader.m_ProgramID, "projection");
		glUniformMatrix4fv(PerspectiveLocation, 1, GL_FALSE, &projection[0][0]);

		
		glBindVertexArray(VAO);
		for (int i = 0; i < sizeof(cubePositions)/sizeof(glm::vec3); i++)
		{
			
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			ModelLocation = glGetUniformLocation(shader.m_ProgramID, "model");
			glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
			
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,&EBO);
	glDeleteVertexArrays(1,&VAO);

	glfwTerminate(); 
	return 0;
}


void FrameSizeBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void InputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		IsLeftRight = false;
		moveupdown += 0.001f;
	}
		
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		IsLeftRight = false;
		moveupdown -= 0.001f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		IsLeftRight = true;
		moveleftright += 0.001f;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		IsLeftRight = true;
		moveleftright -= 0.001f;
	}
		
}
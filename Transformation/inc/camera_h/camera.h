
#include<GLFW/glfw3.h>
#include<glad/glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//摄像机位移方向
enum class Camera_Movements
{
	FORWARDS,
	BACKWARDS,
	LEFT,
	RIGHT
};

//摄像机全局参数初始化值
const float YAW				= -90.0f;//航偏角0°时指向x正方向，-90°相当于旋转到-z轴
const float PITCH			= 0.0f;//俯仰角初始0
const float MOVEMENTSPEED	= 2.5f;
const float SENSITIVITY		= 0.05f;
const float FOV				= 45.0f;

class Camera
{
public:

	//其他属性
	float m_MovementSpeed;
	float m_Sensitivity;
	float m_Fov;

	Camera(glm::vec3 position,glm::vec3 up= glm::vec3(0.0f, 1.0f, 0.0f),float pitch=PITCH,float yaw=YAW);
	Camera(float posX, float posY, float posZ, float upX,float upY,float upZ,float pitch,float yaw);

	glm::mat4 GetViewMatrix();

	void ProcessKeyboard(Camera_Movements direction, double deltaTime); 
	void ProcessMouseCursor(double offsetX, double offsetY, bool PitchConstrain = true);
	void ProcessMouseScroll(double offsetY);

private:	
	//摄像机属性
	glm::vec3 m_Posotion;
	glm::vec3 m_Front;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
	glm::vec3 m_WorldUp;

	//欧拉角
	float m_Yaw;
	float m_Pitch;

	void update();

};

	Camera::Camera(glm::vec3 position, glm::vec3 up, float pitch, float yaw ):m_Posotion(position)
	{
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
		m_WorldUp = up;
		m_Yaw = yaw;
		m_Pitch = pitch;
		m_MovementSpeed = MOVEMENTSPEED;
		m_Sensitivity = SENSITIVITY;
		m_Fov = FOV;
		update();
	}

	Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float pitch, float yaw)
	{
		m_Posotion = glm::vec3(posX, posY, posZ);
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
		m_WorldUp = glm::vec3(upX, upY, upZ);
		m_Yaw = yaw;
		m_Pitch = pitch;
		m_MovementSpeed = MOVEMENTSPEED;
		m_Sensitivity = SENSITIVITY;
		m_Fov = FOV;
		update();
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		return glm::lookAt(m_Posotion, m_Posotion + m_Front, m_Up);
	}

	void Camera::ProcessKeyboard(Camera_Movements direction, double deltaTime)
	{
		float CameraSpeed = m_MovementSpeed * deltaTime;
		
		if (direction == Camera_Movements::FORWARDS)
			m_Posotion += CameraSpeed * m_Front;
		if (direction == Camera_Movements::BACKWARDS)
			m_Posotion -= CameraSpeed * m_Front;
		if (direction == Camera_Movements::LEFT)
			m_Posotion -= CameraSpeed * m_Right;
		if (direction == Camera_Movements::RIGHT)
			m_Posotion += CameraSpeed * m_Right;
	}

	void Camera::ProcessMouseCursor(double offsetX, double offsetY, bool PitchConstrain)
	{
		offsetX *= m_Sensitivity;
		offsetY *= m_Sensitivity;

		m_Yaw += offsetX;
		m_Pitch += offsetY; 

		if(PitchConstrain)
			m_Pitch = m_Pitch >= 85.0f ? 85.0f : (m_Pitch <= -85.0f ? -85.0f : m_Pitch);

		update();
	}

	void Camera::ProcessMouseScroll(double offsetY)
	{
		m_Fov -= offsetY;
		m_Fov = m_Fov > 120.0f ? 120.0f : (m_Fov < 2.0f ? 2.0f : m_Fov);
	}

	void Camera::update()
	{
		//更新Camera向量
		glm::vec3 front = glm::vec3(1.0f);
		front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
		front.y = std::sin(glm::radians(m_Pitch));
		front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

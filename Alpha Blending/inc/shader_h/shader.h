#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<string>
#include<iostream>
#include<fstream>
#include<sstream>

class Shader
{
public:
	unsigned int m_ProgramID;
	Shader(const char* VSPath, const char* FSPath);
	//activate shader
	void use();
	//utility of uniform, 给name所在的uniform变量的位置赋值value
	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value)const;
	void SetFloat(const std::string& name, float value)const;

	void SetVec2(const std::string& name, float x, float y)const;
	void SetVec2(const std::string& name, glm::vec2 value)const;
	void SetVec3(const std::string& name, float x, float y, float z)const;
	void SetVec3(const std::string& name, glm::vec3 value)const;
	void SetVec4(const std::string& name, float x, float y, float z, float w)const;
	void SetVec4(const std::string& name, glm::vec4 value)const;

	void SetMat2(const std::string& name, glm::mat2 value)const;
	void SetMat3(const std::string& name, glm::mat3 value)const;
	void SetMat4(const std::string& name, glm::mat4 value)const;
private:
	void CheckCompileError(unsigned int shader, std::string type);
};

Shader::Shader(const char* VSPath, const char* FSPath)
{
	//文件流中提取shader source code
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		//打开文件
		vShaderFile.open(VSPath);
		fShaderFile.open(FSPath);

		std::stringstream vShaderString, fShaderString;
		//读取文件的buffer内容到stringstream中
		vShaderString << vShaderFile.rdbuf();
		fShaderString << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderString.str();
		fragmentCode = fShaderString.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	//shader source code
	const char* vertexCode_char = vertexCode.data();
	const char* fragmentCode_char = fragmentCode.data();

	//create and compile shaders
	unsigned int vs_ID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs_ID, 1, &vertexCode_char, NULL);
	glCompileShader(vs_ID);
	CheckCompileError(vs_ID, "VERTEX");

	unsigned int fs_ID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs_ID, 1, &fragmentCode_char, NULL);
	glCompileShader(fs_ID);
	CheckCompileError(fs_ID, "FRAGMENT");

	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID,vs_ID);
	glAttachShader(m_ProgramID, fs_ID);
	glLinkProgram(m_ProgramID);
	CheckCompileError(m_ProgramID, "PRORAME");

	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);
}

void Shader::use()
{
	glUseProgram(m_ProgramID);
}


void Shader::SetBool(const std::string& name, bool value)const
{
	unsigned int UniformLocation = glGetUniformLocation(m_ProgramID, name.data());
	glUniform1i(UniformLocation, (int)value);
}

void Shader::SetFloat(const std::string& name, float value)const
{
	unsigned int UniformLocation = glGetUniformLocation(m_ProgramID, name.data());
	glUniform1f(UniformLocation, value);
}

void Shader::SetInt(const std::string& name, int value)const
{
	unsigned int UniformLocation = glGetUniformLocation(m_ProgramID, name.data());
	glUniform1i(UniformLocation, (value));
}


void Shader::SetVec2(const std::string& name, float x, float y)const
{
	glUniform2f(glGetUniformLocation(m_ProgramID, name.data()), x, y);
}

void Shader::SetVec2(const std::string& name, glm::vec2 value)const
{
	glUniform2fv(glGetUniformLocation(m_ProgramID, name.data()),1,&value[0]);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z)const
{
	glUniform3f(glGetUniformLocation(m_ProgramID,name.data()), x, y, z);
}

void Shader::SetVec3(const std::string& name, glm::vec3 value)const
{
	glUniform3fv(glGetUniformLocation(m_ProgramID, name.data()),1,&value[0]);
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w)const
{
	glUniform4f(glGetUniformLocation(m_ProgramID, name.data()), x, y, z,w);
}

void Shader::SetVec4(const std::string& name, glm::vec4 value)const
{
	glUniform4fv(glGetUniformLocation(m_ProgramID, name.data()), 1, &value[0]);
}

void Shader::SetMat2(const std::string& name, glm::mat2 value)const
{
	glUniformMatrix2fv(glGetUniformLocation(m_ProgramID, name.data()), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetMat3(const std::string& name, glm::mat3 value)const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ProgramID, name.data()), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetMat4(const std::string& name, glm::mat4 value)const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.data()), 1, GL_FALSE, &value[0][0]);
}



void Shader::CheckCompileError(unsigned int shader, std::string type)
{
	int success;
	char infoLog[512];

	if (type != "Program")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}

}


#endif
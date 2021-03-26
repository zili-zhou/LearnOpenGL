#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>
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
	//utility of uniform
	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
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


void Shader::SetBool(const std::string& name, bool value)
{
	int UniformLocation = glGetUniformLocation(m_ProgramID, name.data());
	glUniform1i(UniformLocation, (int)value);
}

void Shader::SetFloat(const std::string& name, float value)
{
	int UniformLocation = glGetUniformLocation(m_ProgramID, name.data());
	glUniform1f(UniformLocation, value);
}

void Shader::SetInt(const std::string& name, int value)
{
	int UniformLocation = glGetUniformLocation(m_ProgramID, name.data());
	glUniform1i(UniformLocation, (value));
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
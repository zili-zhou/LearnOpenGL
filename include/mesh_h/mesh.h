#ifndef MESH_H
#define MESH_H

#include<glad/glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<shader_h/shader.h>

#include<string>
#include<vector>


//顶点数据
struct Vertex
{
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec2 m_TexCoordinate;
};


//纹理数据
struct Texture
{
	unsigned int m_ID;
	std::string m_Type;//纹理类型，漫反射贴图/镜面反射贴图
	std::string m_Path;
};


//Mesh类
class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices, std::vector<Texture>& _textures);
	void Draw(Shader& shader);

private:
	unsigned int VAO, VBO, EBO;
	void SetupMesh();//初始化缓冲
};

Mesh::Mesh(std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices, std::vector<Texture>& _textures)
	:vertices(_vertices),indices(_indices),textures(_textures)
{
	SetupMesh();
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)(0));
	glEnableVertexAttribArray(0);
	//顶点法线
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,m_Normal));
	glEnableVertexAttribArray(1);
	//顶点纹理坐标
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_TexCoordinate));
	glEnableVertexAttribArray(2);
}


void Mesh::Draw(Shader& shader)
{
	//镜面反射/漫反射纹理数
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for ( unsigned int i = 0; i < textures.size(); i++)
	{
		// 在绑定纹理之前激活相应的纹理单元
		glActiveTexture(GL_TEXTURE0 + i);
		// 获取纹理序号
		std::string number = "";//纹理序号
		std::string name = textures[i].m_Type;//纹理类型

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		
		shader.SetInt(( name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].m_ID);
	}
	

	//绘制网络
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	glActiveTexture(GL_TEXTURE0);
}




#endif;

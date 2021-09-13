#ifndef MESH_H
#define MESH_H

#include<glad/glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<shader_h/shader.h>

#include<string>
#include<vector>


//��������
struct Vertex
{
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec2 m_TexCoordinate;
};


//��������
struct Texture
{
	unsigned int m_ID;
	std::string m_Type;//�������ͣ���������ͼ/���淴����ͼ
	std::string m_Path;
};


//Mesh��
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
	void SetupMesh();//��ʼ������
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

	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)(0));
	glEnableVertexAttribArray(0);
	//���㷨��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,m_Normal));
	glEnableVertexAttribArray(1);
	//������������
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_TexCoordinate));
	glEnableVertexAttribArray(2);
}


void Mesh::Draw(Shader& shader)
{
	//���淴��/������������
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for ( unsigned int i = 0; i < textures.size(); i++)
	{
		// �ڰ�����֮ǰ������Ӧ������Ԫ
		glActiveTexture(GL_TEXTURE0 + i);
		// ��ȡ�������
		std::string number = "";//�������
		std::string name = textures[i].m_Type;//��������

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		
		shader.SetInt(( name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].m_ID);
	}
	

	//��������
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	glActiveTexture(GL_TEXTURE0);
}




#endif;

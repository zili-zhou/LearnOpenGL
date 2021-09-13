/*
#ifndef MODEL_H
#define MODEL_H


#include<glad/glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<mesh_h/mesh.h>
#include<shader_h/shader.h>

#include<unordered_map>

class Model
{
public:	
	std::vector<Mesh>meshes;
	std::vector<Texture>Loaded_Textures;
	std::string directory;

	Model(const std::string path);
	void Draw(Shader& shader);
	//std::unordered_map<aiString,int>Set_Texture;//存放所有加载的纹理且不重复

private:
	void LoadModel(const std::string &path);
	void ProcessNode(const aiScene* scene, aiNode* node);
	Mesh ProcessMesh(const aiScene* scene, aiMesh* mesh);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string name);
	unsigned int GetTextureIDFromFile(const char* path, const std::string& directory, bool gamma);
};

Model::Model(const std::string path)
{
	LoadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (auto& mesh : meshes)
		mesh.Draw(shader);
}

void Model::LoadModel(const std::string &path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate| aiProcess_FlipUVs);//枚举类型aiPostProcessSteps，可以使用位或操作包含多个选项

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene, scene->mRootNode);
}

//前序遍历从root节点开始的mesh树，并处理每个子节点的mesh
void Model::ProcessNode(const aiScene* scene, aiNode* node)
{
	
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];//scene里的mMeshes容器存放的mesh实体，node里的mMeshes容器存放的是scene存放的mesh实体的索引。
		meshes.push_back(ProcessMesh(scene, mesh));//处理好的mesh传入meshes数组
	}

	//处理子节点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(scene, node->mChildren[i]);
}


Mesh Model::ProcessMesh(const aiScene* scene, aiMesh* mesh)
{
	//用于填充Mesh的对象
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//处理顶点
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//填充顶点对象
		Vertex vertex = {};
		
		glm::vec3 vector=glm::vec3(0.0f);
		//顶点位置
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.m_Position = vector;
		//顶点法线
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.m_Normal = vector;
		//顶点纹理坐标
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 tex = glm::vec2(0.0f);
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			vertex.m_TexCoordinate = tex;
		}
		else
			vertex.m_TexCoordinate = glm::vec2(0.0f);
		vertices.push_back(vertex);
	}

	//处理索引
	//遍历mesh的每个三角形面，遍历每个面的所有索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
		
	}

	//处理材质
	if (mesh->mMaterialIndex >= 0)//一个网格只包含了一个指向材质对象的索引，mMaterialIndex是scene中mMaterials数组的索引
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//载入diffuse纹理并添加到textures数组
		std::vector<Texture> diffuseMap = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());

		//载入specular纹理并添加到textures数组
		std::vector<Texture> specularMap = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());
	}

	return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string name)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString filestr;//纹理文件位置
		material->GetTexture(type, i, &filestr);//获取纹理的位置

		bool skip = false;

		for (unsigned int j = 0; j < Loaded_Textures.size(); j++)
		{
			if (std::strcmp(Loaded_Textures[j].m_Path.C_Str(), filestr.C_Str()) == 0)
			{
				textures.push_back(Loaded_Textures[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.m_ID = GetTextureIDFromFile(filestr.C_Str(), this->directory, false);
			texture.m_Path = filestr.C_Str();
			texture.m_Type = type;
			textures.push_back(texture);
			Loaded_Textures.push_back(texture);
		}

	}
	return textures;
}


unsigned int Model::GetTextureIDFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = "";
	filename = directory + '/' + std::string(path);

	unsigned int texture;
	glGenTextures(1, &texture);

	int Width, Height, ColorChannel;
	unsigned char* data = stbi_load(filename.data(), &Width, &Height, &ColorChannel, 0);

	if (data != nullptr)
	{
		GLenum format;
		if (ColorChannel == 1)
			format = GL_RED;
		else if (ColorChannel == 3)
			format = GL_RGB;
		else if (ColorChannel == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cerr << "Failed to load Texture." << std::endl;
		stbi_image_free(data);
	}
	
	return texture;
}


#endif
*/
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh_h/mesh.h>
#include <shader_h/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const aiString& path,  string& directory, bool gamma = false);

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.m_Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.m_Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.m_TexCoordinate = vec;
                /*
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
                */
            }
            else
                vertex.m_TexCoordinate = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {/*
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].m_Path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                auto a = str.data;
                texture.m_ID = TextureFromFile(str, this->directory);
                texture.m_Type = typeName;
                texture.m_Path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }*/
            aiString str;
            mat->GetTexture(type, i, &str);
            Texture texture;
            texture.m_ID = TextureFromFile(str, directory);
            texture.m_Type = typeName;
            texture.m_Path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
        return textures;
    }
};


unsigned int TextureFromFile(const aiString& path,  string& directory, bool gamma)
{
    string name = ""; 
    for (const auto& c : path.data)
    {
        if (c == '\0')
            continue;
        if (c == '\x1b')
            break;
        name += c;
    }
    auto filename = directory +'/'+name;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.data(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif

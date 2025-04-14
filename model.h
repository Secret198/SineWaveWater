#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "mesh.h"
#include "stb_image.h"

class Model {
public:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	GLenum textureType;
	unsigned int textureId;

	Model(std::string const path) {
		loadModel(path);
	}

	void Draw(Shader& shader, bool customTexture) {
		for (int i = 0; i < meshes.size(); i++)
		{
			if (!customTexture) {
				meshes[i].Draw(shader);
			}
			else {
				meshes[i].Draw(shader, textureType, textureId);
			}
		}
	}

	void Draw(Shader& shader, GLenum type, unsigned int texId) {
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader, type, texId);
		}
	}

	void loadCubeMap(std::vector<std::string> faces) {
		unsigned int textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

		int width, height, nbrOfChannels;
		for (unsigned int i = 0; i < faces.size(); i++) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nbrOfChannels, 0);
			if (data) {
				GLenum format;
				if (nbrOfChannels == 3) {
					format = GL_RGB;
				}
				else if (nbrOfChannels == 4) {
					format = GL_RGBA;
				}
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else {
				std::cout << "Failed to load cubemap at path: " << faces[i] << std::endl;
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		}

		this->textureId = textureId;
		this->textureType = GL_TEXTURE_CUBE_MAP;
	}

	
	
private:


	void loadModel(std::string const path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP" << import.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of("/"));

		processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indicies;
		std::vector<Texture> textures;

		//Reading vertex positions and normal vectors
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 tempVector;
			tempVector.x = mesh->mVertices[i].x;
			tempVector.y = mesh->mVertices[i].y;
			tempVector.z = mesh->mVertices[i].z;
			vertex.Position = tempVector;

			if (mesh->HasNormals()) {
				tempVector.x = mesh->mNormals[i].x;
				tempVector.y = mesh->mNormals[i].y;
				tempVector.z = mesh->mNormals[i].z;
				vertex.Normal = tempVector;
			}


			if (mesh->mTextureCoords[0]) {
				glm::vec2 tempVector2;

				tempVector2.x = mesh->mTextureCoords[0][i].x;
				tempVector2.y = mesh->mTextureCoords[0][i].y;

				vertex.TexCoords = tempVector2;
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}

		//Reading indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indicies.push_back(face.mIndices[j]);
			}
		}

		//Reading the materials
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
		
		return Mesh(vertices, indicies, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data, str.C_Str()) == 0) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}

			if (!skip) {
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}


		}

		return textures;
	}

	unsigned int TextureFromFile(const char* path, const std::string& directory) {
		std::string fileName = std::string(path);
		fileName = directory + "/" + fileName;

		unsigned int textureId;
		glGenTextures(1, &textureId);

		int width, height, ComponentsNum;
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &ComponentsNum, 0);

		if (data) {
			GLenum format;
			switch (ComponentsNum)
			{
				case 1:
					format = GL_RED;
					break;
				case 3:
					format = GL_RGB;
					break;
				case 4:
					format = GL_RGBA;
					break;
				default:
					std::cout << "idk what's going on" << std::endl;
					break;
			}

			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureId;
	}
};
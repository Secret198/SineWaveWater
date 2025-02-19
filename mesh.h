#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>
#include <glm-1.0.1/glm/gtc/type_ptr.hpp>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shaderClass.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh {
public:
	//mesh data
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;

	Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indices, std::vector<Texture> textures) {
		this->verticies = verticies;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}

	void Draw(Shader& shader) {
		unsigned int diffuseNum = 1;
		unsigned int specularNum = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse") {
				number += std::to_string(diffuseNum++);
			}
			else if (name == "texture_specular") {
				number += std::to_string(specularNum++);
			}

			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glActiveTexture(GL_TEXTURE0);


		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}


private:
	unsigned int VBO, EBO;

	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), &verticies[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};
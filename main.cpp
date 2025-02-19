#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "shaderClass.h"
#include "stb_image.h"
#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>
#include <glm-1.0.1/glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "model.h"

using namespace std;

int width = 800;
int height = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f;
float pitch = 0.0f;

float lastMouseX = width / 2;
float lastMouseY = height / 2;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouseMove = true;

float fov = 45.0f;

void framebuffer_size_callback(GLFWwindow* window, int Twidth, int Theight) {
	glViewport(0, 0, Twidth, Theight);
}

void process_input(GLFWwindow* window) {

	float camSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouseMove) {
		lastMouseX = xPos;
		lastMouseY = yPos;
		firstMouseMove = false;
	}

	float xOffset = xPos - lastMouseX;
	float yOffset = lastMouseY - yPos;
	lastMouseX = xPos;
	lastMouseY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(static_cast<float>(yOffset));
}


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Water", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to load opengl" << endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, width, height);

	stbi_set_flip_vertically_on_load(true);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader waterShader("D:/projects/Water/SimpleWater/vertexShader.glsl", "D:/projects/Water/SimpleWater/fragmentShader.glsl");
	Model plane("models/plane/plane.obj");

	glUniform3fv(glGetUniformLocation(waterShader.ID, "DirLight.direction"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(10.0f, 10.f, 10.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.9f));

		waterShader.use();
		glUniformMatrix4fv(glGetUniformLocation(waterShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(waterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(waterShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(waterShader.ID, "time"), currentFrame * 2.0f);
		glUniform1f(glGetUniformLocation(waterShader.ID, "time"), deltaTime*2.0f);

		plane.Draw(waterShader);

		process_input(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
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
#include <random>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

int width = 800;
int height = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f;
float pitch = 0.0f;

float lastMouseX = width / 2;
float lastMouseY = height / 2;

Camera camera(glm::vec3(0.0f, 1.0f, -10.0f));
bool firstMouseMove = true;

float fov = 45.0f;

const int numberOfWaves = 20;
const int paramCount = 3;
float randomValues[numberOfWaves * paramCount];

int cursorMode = GLFW_CURSOR_NORMAL;
bool showWindow = false;
int editModeState = 0;

string skyBoxPath = "D:/projects/Water/SimpleWater/textures/skybox/";

vector<string> skyboxFaces = {
	skyBoxPath + "right.jpg",
	skyBoxPath + "left.jpg",
	skyBoxPath + "bottom.jpg",
	skyBoxPath + "top.jpg",
	skyBoxPath + "front.jpg",
	skyBoxPath + "back.jpg",
};

float wavelengthMedian = 2.0f;
float wavelengthMax = wavelengthMedian * 1.5f;
float wavelengthMin = wavelengthMedian * 0.5f;

float maxAngle = 60.0f;

float amplitudeMedian = 0.1f;

glm::vec3 sunDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 sunDiffuse = glm::vec3(1.0f, 0.98f, 0.698f);
glm::vec3 sunSpecular = glm::vec3(1.0f, 0.992f, 0.89f);

void framebuffer_size_callback(GLFWwindow* window, int Twidth, int Theight) {
	glViewport(0, 0, Twidth, Theight);
}

void process_input(GLFWwindow* window) {

	float camSpeed = 2.5f * deltaTime;
	int editKeyState = glfwGetKey(window, GLFW_KEY_G);
	
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
	else if (editKeyState == GLFW_PRESS && editKeyState != editModeState) {
		glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
		cursorMode = (cursorMode == GLFW_CURSOR_DISABLED) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		showWindow = !showWindow;
		editModeState = editKeyState;
		firstMouseMove = !showWindow;
		
	}
	else if (editKeyState == GLFW_RELEASE) {
		editModeState = editKeyState;
	}
	
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouseMove) {
		lastMouseX = xPos;
		lastMouseY = yPos;
		firstMouseMove = false;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(xPos, yPos);
	
	if (!io.WantCaptureMouse && !showWindow) {
		float xOffset = xPos - lastMouseX;
		float yOffset = lastMouseY - yPos;
		lastMouseX = xPos;
		lastMouseY = yPos;

		camera.ProcessMouseMovement(xOffset, yOffset);
	}
	
}


void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(xOffset, yOffset);
}

float generateRandomFloat(float a, float b) {
	random_device device;
	mt19937 gen(device());
	uniform_real_distribution<float> dist(a, b);
	return dist(gen);
}

void GenerateRandomValues(Shader shader) {
	shader.use();

	wavelengthMin = wavelengthMedian * 0.5;
	wavelengthMax = wavelengthMedian * 1.5;

	for (int i = 0; i < numberOfWaves*paramCount; i+=paramCount)
	{
		//randomValues[i] = (float)(rand() % 5) + (float)rand() / (float)(RAND_MAX);
		randomValues[i] = generateRandomFloat(wavelengthMin, wavelengthMax);
		randomValues[i + 1] = generateRandomFloat(0.0f, maxAngle);
		randomValues[i + 2] = amplitudeMedian / wavelengthMedian * randomValues[i];

	}
	glUniform1fv(glGetUniformLocation(shader.ID, "randomValues"), sizeof(randomValues) / sizeof(*randomValues), randomValues);
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();


	stbi_set_flip_vertically_on_load(true);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	
	Shader skyboxShader("D:/projects/Water/SimpleWater/shaders/skyboxVertex.glsl", "D:/projects/Water/SimpleWater/shaders/skyboxFragment.glsl");
	Shader waterShader("D:/projects/Water/SimpleWater/shaders/vertexShader.glsl", "D:/projects/Water/SimpleWater/shaders/fragmentShader.glsl");

	Model skybox("D:/projects/Water/SimpleWater/models/cube/cube.obj");
	Model plane("models/plane/plane.obj");

	waterShader.use();

	skybox.loadCubeMap(skyboxFaces);

	GenerateRandomValues(waterShader);

	glUniform3fv(glGetUniformLocation(waterShader.ID, "lightSource.direction"), 1, glm::value_ptr(sunDirection));
	glUniform3fv(glGetUniformLocation(waterShader.ID, "lightSource.diffusion"), 1, glm::value_ptr(sunDiffuse));
	glUniform3fv(glGetUniformLocation(waterShader.ID, "lightSource.specular"), 1, glm::value_ptr(sunSpecular));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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
		glUniform1f(glGetUniformLocation(waterShader.ID, "time"), currentFrame);
		glUniform1f(glGetUniformLocation(waterShader.ID, "deltaTime"), deltaTime);
		glUniform3fv(glGetUniformLocation(waterShader.ID, "viewPos"), 1, glm::value_ptr(camera.Position));

		plane.Draw(waterShader, false);

		skyboxShader.use();
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		skybox.Draw(skyboxShader, true);

		if (showWindow) {
			ImGui::Begin("Edit window");
			ImGui::Text("Wave settings");
			ImGui::SliderFloat("Wave length median", &wavelengthMedian, 0.0f, 10.0f);
			ImGui::SliderFloat("Max angle", &maxAngle, 0.0f, 90.0f);
			ImGui::SliderFloat("Amplitude median", &amplitudeMedian, 0.0f, 1.0f);
			if (ImGui::Button("Apply")) {
				GenerateRandomValues(waterShader);
			}
			ImGui::Separator();
			ImGui::Text("Light settings");
			ImGui::Text("Sun direction");
			ImGui::PushItemWidth(100.0f);
			ImGui::SliderFloat("X", &sunDirection.x, -1.0f, 1.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Y", &sunDirection.y, -1.0f, 1.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Z", &sunDirection.z, -1.0f, 1.0f);
			ImGui::End();
		}

		//idk about this
		glUniform3fv(glGetUniformLocation(waterShader.ID, "lightSource.direction"), 1, glm::value_ptr(sunDirection));
		glUniform3fv(glGetUniformLocation(waterShader.ID, "lightSource.diffusion"), 1, glm::value_ptr(sunDiffuse));
		glUniform3fv(glGetUniformLocation(waterShader.ID, "lightSource.specular"), 1, glm::value_ptr(sunSpecular));

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwPollEvents();
		process_input(window);
		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

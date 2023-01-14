#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

// Import local files
#include "Shader.h"
#include "Mesh.h"

// global variables
static unsigned int screenshotId = 0;
const unsigned int WINDOW_WIDTH = 1024;
const unsigned int WINDOW_HEIGHT = 768;
const char* WINDOW_NAME = "COMPSCI 3GC3 Assignment 3 -- Khoa Bui \0";

struct SpotLight {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 attenuation;
	glm::vec3 position;
	glm::vec3 direction;
	float cutoffAngle;    // The cosine value of cutoff angle
};

// Function declarations
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
	// Initialize and config glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Mesh timmy("./asset/timmy.obj", "./asset/timmy.png");
	Mesh bucket("./asset/bucket.obj", "./asset/bucket.jpg");
	Mesh floor("./asset/floor.obj", "./asset/floor.jpeg");

	// enable face culling
	glEnable(GL_CULL_FACE);

	// enable depth testing and set depth comparison function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// render in wireframe mode
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Compile vertex & fragment shaders and create a shader program
	Shader shaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

	// Camera settings (position and target vary per task)
	glm::vec3 cameraPos = glm::vec3(50.0f, 100.0f, 200.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 80.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	
	SpotLight spotlights[3];

	// Initialize spotlights (all of them have the same position, ambient, cutoff angle and attenuation)
	for (int i = 0; i < 3; i++) {
		spotlights[i].ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		spotlights[i].attenuation = glm::vec3(1.0f, 0.35f * 1e-4, 0.44 * 1e-4);
		spotlights[i].position = glm::vec3(0.0f, 200.0f, 0.0f);
		spotlights[i].cutoffAngle = glm::cos(M_PI / 6.0f);
	}

	spotlights[0].diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	spotlights[0].direction = glm::vec3(50.0f, -200.0f, 50.0f);

	spotlights[1].diffuse = glm::vec3(0.0f, 1.0f, 0.0f);
	spotlights[1].direction = glm::vec3(-50.0f, -200.0f, -50.0f);

	spotlights[2].diffuse = glm::vec3(0.0f, 0.0f, 1.0f);
	spotlights[2].direction = glm::vec3(0.0f, -200.0f, 50.0f);

	// Setting up transformation matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

	// activate shader program
	shaderProgram.use();

	// Pass model, view and projection matrices to vertex shader
	shaderProgram.setMat4("model", model);
	shaderProgram.setMat4("view", view);
	shaderProgram.setMat4("projection", projection);

	// Send spotlights to shader
	for (int i = 0; i < 3; i++) {
		std::string prefix = "spotlights[" + std::to_string(i) + "].";
		shaderProgram.setVec3(prefix + "ambient", spotlights[i].ambient);
		shaderProgram.setVec3(prefix + "attenuation", spotlights[i].attenuation);
		shaderProgram.setVec3(prefix + "position", spotlights[i].position);
		shaderProgram.setVec3(prefix + "diffuse", spotlights[i].diffuse);
		shaderProgram.setVec3(prefix + "direction", spotlights[i].direction);
		shaderProgram.setFloat(prefix + "cutoffAngle", spotlights[i].cutoffAngle);
	}

	float theta = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// Background color
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rotation matrix
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f));
		theta += 0.05f;

		glm::vec3 newDirection;
		for (int i = 0; i < 3; i++) {
			newDirection = glm::vec3(rotation * glm::vec4(spotlights[i].direction, 1.0f));
			shaderProgram.setVec3("spotlights[" + std::to_string(i) + "].direction", newDirection);
		}

		timmy.render();
		floor.render();
		bucket.render();

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	timmy.deleteBuffers();
	bucket.deleteBuffers();
	floor.deleteBuffers();
	shaderProgram.deleteProgram();
	glfwTerminate();
	return 0;
}

void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
	int pixelChannel = 3;
	int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
	GLubyte* pixels = new GLubyte[totalPixelSize];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	std::string fileName = prefix + std::to_string(screenshotId) + ".ppm";
	std::ofstream fout(fileName);
	fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
	for (size_t i = 0; i < height; i++)
	{
		for (size_t j = 0; j < width; j++)
		{
			size_t cur = pixelChannel * ((height - i - 1) * width + j);
			fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
		}
		fout << std::endl;
	}
	screenshotId++;
	delete[] pixels;
	fout.flush();
	fout.close();
}

void processInput(GLFWwindow* window) {
	// Press escape to exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Press p to capture screen
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		std::cout << "Capture Window " << screenshotId << std::endl;
		int buffer_width, buffer_height;
		glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
		dump_framebuffer_to_ppm("Assignment1-ss", buffer_width, buffer_height);
		std::cout << "Finish capturing Window " << screenshotId - 1 << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
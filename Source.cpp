#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>

// Import local files
#include "Shader.h"
#include "Mesh.h"

// global variables
static unsigned int screenshotId = 0;
const unsigned int WINDOW_WIDTH = 1024;
const unsigned int WINDOW_HEIGHT = 768;
const char* WINDOW_NAME = "COMPSCI 3GC3 Assignment 3 -- Khoa Bui \0";

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

	//Mesh timmy("./asset/timmy.obj", "./asset/timmy.png");
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

	// Setting up transformation matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

	// activate shader program
	shaderProgram.use();
	shaderProgram.setMat4("model", model);
	shaderProgram.setMat4("view", view);
	shaderProgram.setMat4("projection", projection);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// Background color
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		floor.render();
		bucket.render();

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	bucket.deleteBuffers();
	floor.deleteBuffers();
	shaderProgram.deleteProgram();
	glfwTerminate();
	return 0;
}

// Function definitions
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
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define USE_MATH_DEFINES
#include <iomanip>
#include <random>

#include "Objects/skybox/SkyBox.h"
#include "objects/gltf_object/GltfObject.h"

#include "light/Light.h"
#include "camera/camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>


static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow* window, double xPos, double yPos);

// Random preset
std::random_device rd;  
std::mt19937 gen(rd()); 

// View control 
static float viewAzimuth = 0;
static float viewPolar = 0;
static float viewDistance = 10.0f;

// Lighting
Light light(glm::vec3(5e6f, 5e6f, 5e6f), glm::vec3(-275.0f, 500.0f, 800.0f));

// OpenGL camera view parameters
float cameraSensitivity = 0.001f;
Camera camera = Camera(viewAzimuth, viewPolar, viewDistance, cameraSensitivity);

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Final Project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


    // ---------------------------

	glm::float32 FoV = 45;
	glm::float32 zNear = 0.1f;
	glm::float32 zFar = 1000.0f;
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, zNear, zFar);

	auto bot = GltfObject("../final_project/3d_assets/shiba/scene.gltf");
	auto suzanne = GltfObject("../final_project/3d_assets/suzanne/Suzanne.gltf");
	auto skybox = SkyBox();
	bot.setScale(glm::vec3(0.3f));
	bot.setRotation(-90.0f, glm::vec3(1, 0, 0));


	std::vector<GraphicsObject *> objects = {&bot, &skybox, &suzanne};

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	bool playAnimation = true;
	static float playbackSpeed = 2.0f;

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update states for animation
		double currentTime = glfwGetTime();
		auto deltaTime = static_cast<float>(currentTime - lastTime);
		lastTime = currentTime;

		if (playAnimation) {
			time += deltaTime * playbackSpeed;
			bot.update(time);
		}

		glm::mat4 vp = projectionMatrix * camera.getViewMatrix();

		// Render the buildings
		for (auto object : objects)
			object->render(vp, light);

		// FPS tracking
		// Count number of frames over a few seconds and take average
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = static_cast<float>(frames) / fTime;
			frames = 0;
			fTime = 0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Final Project | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	for (auto object : objects)
		object->cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	camera.onKeyPress(window);
}

static void mouse_callback(GLFWwindow* window, double xPos, double yPos){
	camera.onMouseChange(window, xPos, yPos);
}

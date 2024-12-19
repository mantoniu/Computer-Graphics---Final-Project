#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define USE_MATH_DEFINES
#include <iomanip>
#include <random>

#include "Objects/skybox/SkyBox.h"
#include "objects/gltf_object/GltfObject.h"

#include "camera/camera.h"
#include <lighting/light/Light.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>

#include "lighting/lights_manager/LightsManager.h"


static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow* window, double xPos, double yPos);

// OpenGL camera view parameters
float cameraSensitivity = 0.001f;
auto camera = Camera(-3, -0.60, glm::vec3(-6, 40, 68), 0.001, 45, 0.1f, 1500.0f);

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

	auto light = Light(glm::vec3(28, 58, 51), 5000, glm::vec3(1), glm::vec3(0), 90, 5, 100);
	auto light2 = Light(glm::vec3(-57, 22, 31), 3000, glm::vec3(1), glm::vec3(0), 90, 5, 100);

	auto skybox = SkyBox();
	skybox.setScale(glm::vec3(1000));

	auto zombie = GltfObject("../final_project/3d_assets/zombie/untitled.gltf", true);
	zombie.setTranslation(glm::vec3(-17.6, -5.8, 16));
	zombie.setScale(glm::vec3(0.09));
	zombie.setRotation(90, glm::vec3(1,0, 0));

	auto island = GltfObject("../final_project/3d_assets/island/untitled.gltf");
	island.setScale(glm::vec3(30));

	std::vector<GraphicsObject *> objects = {&skybox, &island, &zombie};

	auto lightsManager = LightsManager(std::vector{light, light2}, objects);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	bool playAnimation = true;
	static float playbackSpeed = 2.0f;

	do
	{
		skybox.setTranslation(camera.getPosition());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update states for animation
		double currentTime = glfwGetTime();
		auto deltaTime = static_cast<float>(currentTime - lastTime);
		lastTime = currentTime;

		if (playAnimation) {
			time += deltaTime * playbackSpeed;
			zombie.update(time);
		}

		glm::mat4 vp = camera.getVPMatrix();

		// Compute depths textures
		lightsManager.render();

		// Render the buildings
		for (auto object : objects)
			object->render(vp);

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

	lightsManager.clean();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	camera.onKeyPress(window);
}

static void mouse_callback(GLFWwindow* window, const double xPos, const double yPos){
	camera.onMouseChange(window, static_cast<float>(xPos), static_cast<float>(yPos));
}

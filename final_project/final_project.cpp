#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define USE_MATH_DEFINES
#include <iomanip>
#include <random>

#include "Objects/skybox/SkyBox.h"
#include "objects/gltf_object/GltfObject.h"

#include "camera/camera.h"
#include <light/Light.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/detail/type_vec.hpp>

#include "passes/geometry_pass/GeometryPass.h"
#include "passes/lighting_pass/LightingPass.h"
#include "passes/ssao_blur_pass/SSAOBlurPass.h"
#include "passes/ssao_pass/SSAOPass.h"

#define WIDTH 1024
#define HEIGHT 768

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow* window, double xPos, double yPos);

// OpenGL camera view parameters
float cameraSensitivity = 0.001f;
auto camera = Camera(-3.233, -0.535, glm::vec3(-6, 40, 68), 0.001, 45, 0.1f, 1800.0f);

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
	window = glfwCreateWindow(WIDTH, HEIGHT, "Final Project", nullptr, nullptr);
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
	if (int version = gladLoadGL(glfwGetProcAddress); version == 0) {
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    // ---------------------------
	auto light = Light(glm::vec3(40, 21, 22), 1000, glm::vec3(1), glm::vec3(0), 90, 5, 100);
	auto light2 = Light(glm::vec3(-49, 24, -2), 1000, glm::vec3(1), glm::vec3(0), 90, 5, 100);

	auto skybox = SkyBox();
	skybox.setScale(glm::vec3(1000));

	auto zombie = GltfObject("../final_project/3d_assets/zombie/untitled.gltf", true);
	zombie.setTranslation(glm::vec3(-8.16, 3.7, 13.76));
	zombie.setScale(glm::vec3(0.09));
	zombie.setRotation(90, glm::vec3(1,0, 0));

	auto island = GltfObject("../final_project/3d_assets/island/untitled.gltf");
	island.setScale(glm::vec3(30));

	std::vector<GraphicsObject *> objects = {&skybox, &zombie, &island};
	std::vector lights = {light, light2};

	//auto lightsManager = LightsManager(std::vector{light, light2}, objects);

	// Passes
	auto geometryPass = GeometryPass(WIDTH, HEIGHT);
	auto ssaoPass = SSAOPass(WIDTH, HEIGHT, geometryPass);
	auto ssaoBlurPass = SSAOBlurPass(WIDTH, HEIGHT, ssaoPass);
	auto depthPass = DepthPass(WIDTH, HEIGHT, lights);
	auto lightingPass = LightingPass(WIDTH, HEIGHT, lights, geometryPass, ssaoBlurPass, depthPass);

	std::vector<RenderPass *> passes = {&geometryPass, &ssaoPass, &ssaoBlurPass, &depthPass, &lightingPass};

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	static float playbackSpeed = 2.0f;

	for (const auto &pass: passes)
		pass->setup();

	do
	{
		skybox.setTranslation(camera.getPosition());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		for (const auto &pass: passes)
			pass->render(objects, camera);

		// Update states for animation
		double currentTime = glfwGetTime();
		auto deltaTime = static_cast<float>(currentTime - lastTime);
		lastTime = currentTime;
		time += deltaTime * playbackSpeed;
		zombie.update(time);

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
	for (const auto &object: objects)
		object->cleanup();

	for (const auto &pass: passes)
		pass->cleanup();

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

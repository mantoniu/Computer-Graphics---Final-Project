#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define USE_MATH_DEFINES
#include <iomanip>
#include <random>

#include "3D_objects/skybox/SkyBox.h"
#include "3D_objects/gltf_object/GltfObject.h"

#include "view_points/camera/camera.h"
#include <view_points/lights/light/Light.h>
#include <glm/detail/type_vec.hpp>

#include "view_points/lights/spot_light/Spotlight.h"
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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For macOS
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
	auto spotLight1 = Spotlight(glm::vec3(-5.65517, 3.35241, 18.2637), 2, glm::vec3(1), -9.90, 0.154003, 20, 30);
	auto spotLight2 = Spotlight(glm::vec3(-0.308003, 3.41245, 19.8179), 2, glm::vec3(1), -3.22801, 0.209002, 20, 30);
	auto spotLight3 = Spotlight(glm::vec3(-8.88, 3.41245, 15.768), 2, glm::vec3(1), -3.88499, 0.132, 20, 30);
	auto spotLight4 = Spotlight(glm::vec3(4.09714, 3.4899, 19.5193), 2, glm::vec3(1), -2.84999, 0.098001, 20, 30);
	auto light = Light(glm::vec3(106.438, 57.1682, 9.46872), 1500, glm::vec3(1), -1.66101, -0.453002);

	auto skybox = SkyBox();
	skybox.setScale(glm::vec3(1000));

	auto zombie = GltfObject("../final_project/3D_assets/alien/alien.gltf", true);
	zombie.setTranslation(glm::vec3(-2, 3.36, 14.8));
	zombie.setScale(glm::vec3(0.003));
	zombie.setRotation(90, glm::vec3(1,0, 0));

	auto island = GltfObject("../final_project/3D_assets/island/island.gltf");
	island.setScale(glm::vec3(30));

	std::vector<GraphicsObject *> objects = {&zombie, &island, &skybox};
	std::vector<Light *> lights = {&spotLight1, &spotLight2, &spotLight3, &spotLight4, &light};

	// Passes
	auto geometryPass = GeometryPass(WIDTH, HEIGHT);
	auto ssaoPass = SSAOPass(WIDTH, HEIGHT, geometryPass);
	auto ssaoBlurPass = SSAOBlurPass(WIDTH, HEIGHT, ssaoPass);
	auto depthPass = DepthPass(8192, 8192, lights);
	auto lightingPass = LightingPass(WIDTH, HEIGHT, lights, geometryPass, ssaoBlurPass, depthPass);

	std::vector<RenderPass *> passes = {&geometryPass, &ssaoPass, &ssaoBlurPass, &depthPass, &lightingPass};

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	static float playbackSpeed = 1.0f;

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

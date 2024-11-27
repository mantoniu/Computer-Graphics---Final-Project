#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera{
    private:
        bool firstMouse = true;         
        float lastX, lastY;    
        float sensitivity;  

        glm::vec3 eyeCenter;
        glm::vec3 up = glm::vec3(0,1,0);

        float viewAzimuth;
        float viewPolar;
        float viewDistance;

        void updateEyeCenter();
    public:
        Camera(float viewAzimuth, float viewPolar, float viewDistance, float sensitivity);
        void onKeyPress(GLFWwindow *window);
        void onMouseChange(GLFWwindow* window, double xpos, double ypos);
        glm::mat4 getViewMatrix();
        void updateViewAnglesFromEyePosition();
        glm::vec3 getLookat();
        glm::vec3 getUp();
};
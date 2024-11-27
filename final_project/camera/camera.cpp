#include "camera.h"
#include "iostream"

Camera::Camera(float viewAzimuth, float viewPolar, float viewDistance, float sensitivity){
    this->eyeCenter = glm::vec3(0,0,0);
    this->viewAzimuth = viewAzimuth;
    this->viewPolar = viewPolar;
    this->viewDistance = viewDistance;
    this->sensitivity = sensitivity;
}

glm::vec3 Camera::getLookat(){
    return glm::vec3(cos(viewPolar)*sin(viewAzimuth),sin(viewPolar), cos(viewPolar)*cos(viewAzimuth)) + eyeCenter;
}

glm::vec3 Camera::getUp(){
    return up;
}

glm::mat4 Camera::getViewMatrix(){
    return glm::lookAt(eyeCenter, getLookat(), getUp());
}


void Camera::onKeyPress(GLFWwindow *window) {
    const float moveSpeed = 0.5f;
    glm::vec3 forward = normalize(getLookat() - eyeCenter);
    glm::vec3 right = normalize(cross(forward, getUp()));

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        eyeCenter = glm::vec3(0.0f, 0.0f, 10.0f);
        std::cout << "Position reset." << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        eyeCenter += forward * moveSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        eyeCenter -= forward * moveSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        eyeCenter -= right * moveSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        eyeCenter += right * moveSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


void Camera::onMouseChange(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    viewAzimuth -= xOffset;
    viewPolar += yOffset;

    if (viewPolar > glm::radians(89.0f)) viewPolar = glm::radians(89.0f);
    if (viewPolar < glm::radians(-89.0f)) viewPolar = glm::radians(-89.0f);
}

#include "camera.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "iostream"

Camera::Camera(const float viewAzimuth, const float viewPolar, const glm::vec3 eyeCenter, const float sensitivity){
    this->eyeCenter = eyeCenter;
    this->viewAzimuth = viewAzimuth;
    this->viewPolar = viewPolar;
    this->sensitivity = sensitivity;
}

glm::vec3 Camera::getLookAt() const {
    return glm::vec3(std::cos(viewPolar)*std::sin(viewAzimuth),std::sin(viewPolar), std::cos(viewPolar)*std::cos(viewAzimuth)) + eyeCenter;
}

glm::vec3 Camera::getUp() const {
    return up;
}

glm::mat4 Camera::getViewMatrix() const {
    return lookAt(eyeCenter, getLookAt(), getUp());
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(FoV), 4.0f / 3.0f, zNear, zFar);
}

glm::mat4 Camera::getVPMatrix() const {
     return getProjectionMatrix() * getViewMatrix();
}

void Camera::onKeyPress(GLFWwindow *window) {
    constexpr float moveSpeed = 0.5f;
    const glm::vec3 forward = normalize(getLookAt() - eyeCenter);
    const glm::vec3 right = normalize(cross(forward, getUp()));

    static bool cKeyWasPressed = false;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!cKeyWasPressed) {
            std::cout << "Camera debug " << (debug ? "disabled" : "enabled") << std::endl;
            debug = !debug;
        }
        cKeyWasPressed = true;
    } else {
        cKeyWasPressed = false;
    }

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
    displayData();
}


void Camera::onMouseChange(GLFWwindow* window, const float xPos, const float yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    viewAzimuth -= xOffset;
    viewPolar += yOffset;

    if (viewPolar > glm::radians(89.0f)) viewPolar = glm::radians(89.0f);
    if (viewPolar < glm::radians(-89.0f)) viewPolar = glm::radians(-89.0f);
    displayData();
}

void Camera::displayData() const {
    if (debug)
        std::cout << "Position = [x: " << eyeCenter.x << ", y: " << eyeCenter.y << ", z: " << eyeCenter.z << "] - "
        << "View Polar : " << viewPolar << ", View azimuth : " << viewAzimuth << std::endl;
}
#include "camera.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "iostream"

Camera::Camera(const float viewAzimuth, const float viewPolar, const glm::vec3 eyeCenter, const float sensitivity, const float FoV, const float zNear, const float zFar) : ViewPoint(eyeCenter, viewAzimuth, viewPolar, FoV, zNear, zFar){
    this->sensitivity = sensitivity;
}

void Camera::onKeyPress(GLFWwindow *window) {
    const glm::vec3 forward = normalize(getLookAt() - getPosition());
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

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        moveSpeed = 0.01f;
    }
    else moveSpeed = 0.5f;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
        std::cout << "Position reset." << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        setPosition(getPosition() + forward * moveSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        setPosition(getPosition() - forward * moveSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        setPosition(getPosition() - right * moveSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        setPosition(getPosition() + right * moveSpeed);
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

    setViewAzimuth(getViewAzimuth() - xOffset);
    setViewPolar(getViewPolar() + yOffset);

    if (getViewPolar() > glm::radians(89.0f)) setViewPolar(glm::radians(89.0f));
    if (getViewPolar() < glm::radians(-89.0f)) setViewPolar(glm::radians(-89.0f));
    displayData();
}

void Camera::displayData() const {
    if (debug)
        std::cout << "Position = [x: " << getPosition().x << ", y: " << getPosition().y << ", z: " << getPosition().z << "] - "
        << "View Polar : " << getViewPolar() << ", View azimuth : " << getViewAzimuth() << std::endl;
}

float Camera::getAspectRatio() const {
    return 4.0f/3.0f;
}

//
// Created by miche on 07/12/2024.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include "glad/gl.h"
#include "GLFW/glfw3.h"

class Camera{
    private:
        bool debug = false;

        glm::float32 FoV;
        glm::float32 zNear;
        glm::float32 zFar;

        bool firstMouse = true;         
        float lastX = -1, lastY = -1;
        float sensitivity;  

        glm::vec3 eyeCenter;
        glm::vec3 up = glm::vec3(0,1,0);
        float viewAzimuth;
        float viewPolar;

        void updateEyeCenter();

    public:
        Camera(float viewAzimuth, float viewPolar, glm::vec3 eyeCenter, float sensitivity, float FoV, float zNear, float zFar);
        void onKeyPress(GLFWwindow *window);
        void onMouseChange(GLFWwindow *window, float xPos, float yPos);

        void displayData() const;

        [[nodiscard]] glm::mat4 getVPMatrix() const;
        [[nodiscard]] glm::vec3 getLookAt() const;
        [[nodiscard]] glm::vec3 getUp() const;
        [[nodiscard]] glm::vec3 getPosition() const;

        [[nodiscard]] glm::mat4 getViewMatrix() const;

        [[nodiscard]] glm::mat4 getProjectionMatrix() const;
};

#endif
//
// Created by miche on 07/12/2024.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "view_points/view_point/ViewPoint.h"

class Camera : public ViewPoint{
    private:
        bool debug = false;

        bool firstMouse = true;         
        float lastX = -1, lastY = -1;
        float sensitivity;
        float moveSpeed = 0.5f;

        void updateEyeCenter();

    public:
        Camera(float viewAzimuth, float viewPolar, glm::vec3 eyeCenter, float sensitivity, float FoV, float zNear, float zFar);
        void onKeyPress(GLFWwindow *window);
        void onMouseChange(GLFWwindow *window, float xPos, float yPos);

        void displayData() const;
        float getAspectRatio() const override;
};

#endif
//
// Created by miche on 24/12/2024.
//

#include "ViewPoint.h"
#include <valarray>
#include <glm/gtc/matrix_transform.hpp>


ViewPoint::ViewPoint(const glm::vec3 position, const float viewAzimuth, const float viewPolar, const float fov, const float near, const float far) : position(position), viewAzimuth(viewAzimuth), viewPolar(viewPolar), fov(fov), near(near), far(far) {
}

glm::vec3 ViewPoint::getLookAt() const {
    return glm::vec3(
        std::cos(viewPolar) * std::sin(viewAzimuth),
        std::sin(viewPolar),
        std::cos(viewPolar) * std::cos(viewAzimuth)
    ) + getPosition();
}

glm::vec3 ViewPoint::getPosition() const{
    return position;
}

float ViewPoint::getViewAzimuth() const {
    return viewAzimuth;
}

float ViewPoint::getViewPolar() const {
    return viewPolar;
}

glm::vec3 ViewPoint::getUp() const {
    return up;
}

void ViewPoint::setPosition(const glm::vec3 position){
    this->position = position;
}

void ViewPoint::setViewAzimuth(const float viewAzimuth) {
    this->viewAzimuth = viewAzimuth;
}

void ViewPoint::setViewPolar(const float viewPolar) {
    this->viewPolar = viewPolar;
}

glm::mat4 ViewPoint::getViewMatrix() const {
    return lookAt(getPosition(), getLookAt(), getUp());
}

glm::mat4 ViewPoint::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), 4.0f / 3.0f, near, far);
}

glm::mat4 ViewPoint::getVPMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}

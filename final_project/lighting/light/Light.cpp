//
// Created by miche on 29/11/2024.
//

#include "Light.h"

#include <glm/gtc/matrix_transform.hpp>

Light::Light(glm::vec3 lightPosition, float lightIntensity, glm::vec3 lightColor, glm::vec3 lookAt, float fov, float depthNear, float depthFar){
    this->lightPosition = lightPosition;
    this->lightIntensity = lightIntensity;
    this->lightColor = lightColor;
    this->lookAt = lookAt;
    this->fov = fov;
    this->depthNear = depthNear;
    this->depthFar = depthFar;
}

float Light::getIntensity() const {
    return lightIntensity;
}

glm::vec3 Light::getPosition() const {
    return lightPosition;
}

glm::vec3 Light::getLightColor() const {
    return lightColor;
}

glm::vec3 Light::getLookAt() const {
    return lookAt;
}

float Light::getDepthNear() const {
    return depthNear;
}

float Light::getDepthFar() const {
    return depthFar;
}

float Light::getFov() const {
    return fov;
}

glm::mat4 Light::getSpaceMatrix() const {
    const glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(getFov()), 1.0f, getDepthNear(), getDepthFar());

    const glm::mat4 lightViewMatrix = glm::lookAt(getPosition(), getLookAt(), glm::vec3(0, 0, 1));
    return lightProjectionMatrix * lightViewMatrix;
}

lightStruct Light::toStruct() const {
    lightStruct lStruct;
    lStruct.position = getPosition();
    lStruct.intensity = getIntensity();
    lStruct.color = getLightColor();
    lStruct.spaceMatrix = getSpaceMatrix();
    return lStruct;
}
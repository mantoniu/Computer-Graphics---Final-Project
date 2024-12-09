//
// Created by miche on 29/11/2024.
//

#include "Light.h"

Light::Light(const glm::vec3 lightIntensity, const glm::vec3 lightPosition) {
    this->lightIntensity = lightIntensity;
    this->lightPosition = lightPosition;
}

glm::vec3 Light::getIntensity() const {
    return lightIntensity;
}

glm::vec3 Light::getPosition() const {
    return lightPosition;
}

lightStruct Light::toStruct() const {
    return {lightIntensity,lightPosition};
}
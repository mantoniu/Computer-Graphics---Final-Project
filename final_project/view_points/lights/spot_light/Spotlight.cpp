//
// Created by miche on 23/12/2024.
//

#include "Spotlight.h"
#include <glm/gtc/matrix_transform.hpp>

Spotlight::Spotlight(glm::vec3 lightPosition, float lightIntensity, glm::vec3 lightColor, float viewAzimuth,
                     float viewPolar, float innerConeAngle, float outerConeAngle): Light(
    lightPosition, lightIntensity, lightColor, viewAzimuth, viewPolar) {
    this->innerConeAngle = glm::radians(innerConeAngle);
    this->outerConeAngle = glm::radians(outerConeAngle);
}

float Spotlight::getInnerConeAngle() const {
    return innerConeAngle;
}

float Spotlight::getOuterConeAngle() const {
    return outerConeAngle;
}

LightTypes Spotlight::getType() const {
    return SPOT_LIGHT;
}

lightStruct Spotlight::toStruct() const {
    lightStruct sStruct = Light::toStruct();
    sStruct.innerConeAngle = getInnerConeAngle();
    sStruct.outerConeAngle = getOuterConeAngle();
    sStruct.direction = normalize(getLookAt() - getPosition());
    return sStruct;
}

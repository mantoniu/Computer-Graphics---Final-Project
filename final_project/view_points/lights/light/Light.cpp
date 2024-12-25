//
// Created by miche on 29/11/2024.
//

#include "Light.h"

#include <glm/gtc/matrix_transform.hpp>
#include "view_points/lights/LightTypes.h"

Light::Light(const glm::vec3 lightPosition, const float lightIntensity, const glm::vec3 lightColor, float viewAzimuth,
             float viewPolar) : ViewPoint(lightPosition, viewAzimuth, viewPolar, 130, 5, 250) {
    this->lightIntensity = lightIntensity;
    this->lightColor = lightColor;
    this->lookAt = lookAt;
}

float Light::getIntensity() const {
    return lightIntensity;
}

glm::vec3 Light::getColor() const {
    return lightColor;
}

LightTypes Light::getType() const {
    return POINT_LIGHT;
}

lightStruct Light::toStruct() const {
    lightStruct lStruct;
    lStruct.type = getType();
    lStruct.position = getPosition();
    lStruct.intensity = getIntensity();
    lStruct.color = getColor();
    lStruct.spaceMatrix = getVPMatrix();
    lStruct.padding1 = 0;
    lStruct.padding2 = 0;
    lStruct.innerConeAngle = 0;
    lStruct.outerConeAngle = 0;
    lStruct.direction = glm::vec3(0, 0, 0);
    lStruct.padding3 = 0;
    return lStruct;
}

float Light::getAspectRatio() const {
    return 1.0f;
}

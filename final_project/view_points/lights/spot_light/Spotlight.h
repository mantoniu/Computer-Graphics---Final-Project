//
// Created by miche on 23/12/2024.
//

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include <tiny_gltf.h>
#include <glm/detail/type_vec.hpp>

#include "../light/Light.h"
#include "view_points/lights/LightTypes.h"

class Spotlight : public Light {
    float innerConeAngle;
    float outerConeAngle;

public:
    Spotlight(glm::vec3 lightPosition, float lightIntensity, glm::vec3 lightColor, float viewAzimuth, float viewPolar,
              float innerConeAngle, float outerConeAngle);

    [[nodiscard]] float getInnerConeAngle() const;

    [[nodiscard]] float getOuterConeAngle() const;

    [[nodiscard]] lightStruct toStruct() const override;

    [[nodiscard]] LightTypes getType() const override;
};

#endif //SPOTLIGHT_H

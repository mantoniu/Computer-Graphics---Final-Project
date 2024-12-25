//
// Created by miche on 29/11/2024.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

#include "../LightTypes.h"
#include "view_points/view_point/ViewPoint.h"

struct lightStruct {
    glm::vec3 position;
    float intensity;

    glm::vec3 color;
    float padding1;

    glm::mat4 spaceMatrix;

    int type;
    float innerConeAngle;
    float outerConeAngle;
    float padding2;

    glm::vec3 direction;
    float padding3;
};

class Light : public ViewPoint{
    private:
        float lightIntensity;
        glm::vec3 lightColor;
        glm::vec3 lookAt;

    public:
        ~Light() override = default;

        Light(glm::vec3 lightPosition, float lightIntensity, glm::vec3 lightColor, float viewAzimuth, float viewPolar);

        [[nodiscard]] float getIntensity() const;
        [[nodiscard]] glm::vec3 getColor() const;
        [[nodiscard]] virtual LightTypes getType() const;
        [[nodiscard]] virtual lightStruct toStruct() const;
        [[nodiscard]] float getAspectRatio() const override;
};



#endif //LIGHT_H

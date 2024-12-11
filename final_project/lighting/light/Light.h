//
// Created by miche on 29/11/2024.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

struct lightStruct {
    glm::vec3 position;
    float intensity;
    glm::vec3 color;
    float padding1;
    glm::mat4 spaceMatrix;
};

class Light {
    private:
        glm::vec3 lightPosition;
        float lightIntensity;
        glm::vec3 lightColor;

        glm::vec3 lookAt;
        float fov;
        float depthNear;
        float depthFar;

    public:
        Light(glm::vec3 lightPosition, float lightIntensity, glm::vec3 lightColor, glm::vec3 lookAt, float fov, float depthNear, float depthFar);

        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] float getIntensity() const;
        [[nodiscard]] glm::vec3 getLightColor() const;
        [[nodiscard]] glm::vec3 getLookAt() const;
        [[nodiscard]] glm::mat4 getSpaceMatrix() const;

        [[nodiscard]] float getFov() const;
        [[nodiscard]] float getDepthNear() const;
        [[nodiscard]] float getDepthFar() const;

        [[nodiscard]] lightStruct toStruct() const;
};



#endif //LIGHT_H

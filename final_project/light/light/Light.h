//
// Created by miche on 29/11/2024.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

struct lightStruct {
    alignas(16) glm::vec3 intensity;
    alignas(16) glm::vec3 position;
};

class Light {
    private:
        glm::vec3 lightIntensity;
        glm::vec3 lightPosition;

    public:
        Light(glm::vec3 lightIntensity, glm::vec3 lightPosition);

        [[nodiscard]] glm::vec3 getIntensity() const;
        [[nodiscard]] glm::vec3 getPosition() const;

        lightStruct toStruct() const;
};



#endif //LIGHT_H

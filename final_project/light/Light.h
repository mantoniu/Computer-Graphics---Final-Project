//
// Created by miche on 29/11/2024.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

class Light {
    private:
        glm::vec3 lightIntensity;
        glm::vec3 lightPosition;

    public:
        Light(glm::vec3 lightIntensity, glm::vec3 lightPosition);
        glm::vec3 getIntensity() const;
        glm::vec3 getPosition() const;
};



#endif //LIGHT_H

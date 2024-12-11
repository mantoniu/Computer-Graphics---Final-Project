//
// Created by miche on 27/11/2024.
//

#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H
#include <string>
#include <glm/detail/type_vec.hpp>
#include <lighting/light/Light.h>
#include "glad/gl.h"

class GraphicsObject {
    private:
        glm::vec3 translation = glm::vec3(0.0f);
        float rotation = 0.0f;
        glm::vec3 scale = glm::vec3(1.0f);
        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        GLuint programId;
        GLuint mvpMatrixID;
        GLuint modelMatrixID;

    public:
        explicit GraphicsObject(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

        virtual ~GraphicsObject() = default;

        [[nodiscard]] glm::mat4 getModelMatrix() const;
        void setTranslation(glm::vec3 translation);
        void setRotation(float rotation, glm::vec3 rotationAxis);
        void setScale(glm::vec3 scale);
        [[nodiscard]] GLuint getProgramId() const;

        virtual void render(glm::mat4 & cameraMatrix) = 0;
        virtual void cleanup();
};

#endif //GRAPHICS_OBJECT_H

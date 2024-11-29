//
// Created by miche on 27/11/2024.
//

#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H
#include <glm/detail/type_mat.hpp>
#include <light/Light.h>

class GraphicsObject {
    public:
        virtual ~GraphicsObject() = default;

        virtual void render(glm::mat4 &cameraMatrix, Light light) = 0;
        virtual void cleanup() = 0;
};

#endif //GRAPHICS_OBJECT_H

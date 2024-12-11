//
// Created by miche on 07/12/2024.
//

#ifndef LIGHTS_MANAGER_H
#define LIGHTS_MANAGER_H
#include <vector>
#include <lighting/light/Light.h>

#include "glad/gl.h"
#include "objects/graphics_object/GraphicsObject.h"


class LightsManager {
    private:
        // Depth perspective settings
        float depthFoV = 90.0f;
        float depthNear = 1.0f;
        float depthFar = 50.0f;

        GLuint depthTexturesArray = -1;
        GLuint depthFBO = -1;
        GLuint lightsUBO;
        std::vector<Light> lights;
        std::vector<GraphicsObject *> objects;

        void loadDepthTextures();
        void loadLightsUBOs();

        void createDepthTextureArray();

    public:
        explicit LightsManager(const std::vector<Light> &lights, const std::vector<GraphicsObject *>& objects);

        void render();
        void clean() const;
};



#endif //LIGHTS_MANAGER_H

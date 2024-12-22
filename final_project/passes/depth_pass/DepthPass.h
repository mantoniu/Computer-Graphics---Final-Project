//
// Created by miche on 22/12/2024.
//

#ifndef DEPTHPASS_H
#define DEPTHPASS_H
#include <light/Light.h>
#include "passes/render_pass/RenderPass.h"

class DepthPass : public RenderPass {
    GLuint depthTexturesArray = 0;
    GLuint lightsUBO = 0;
    std::vector<Light> &lights;

public:
    DepthPass(int width, int height, std::vector<Light> &lights);

    void createDepthTextureArray();

    void setup() override;

    void render(const std::vector<GraphicsObject *> &objects, const Camera &camera) override;

    void cleanup() override;

    [[nodiscard]] GLuint getDepthTexturesArray() const;
};

#endif //DEPTHPASS_H

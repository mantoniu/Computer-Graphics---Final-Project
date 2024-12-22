//
// Created by miche on 22/12/2024.
//

#ifndef LIGHTINGPASS_H
#define LIGHTINGPASS_H
#include "passes/depth_pass/DepthPass.h"
#include "passes/geometry_pass/GeometryPass.h"
#include "passes/ssao_blur_pass/SSAOBlurPass.h"


class LightingPass : public RenderPass{
    GLuint lightsUBO = 0;

    GeometryPass &geometryPass;
    SSAOBlurPass &ssaoBlurPass;
    DepthPass &depthPass;
    std::vector<Light> &lights;

    void loadLightsUBOs();

    public:
        LightingPass(int width, int height, std::vector<Light> &lights, GeometryPass &geometryPass, SSAOBlurPass &ssaoBlurPass, DepthPass &depthPass);

        void setup() override;
        void render(const std::vector<GraphicsObject*>& objects, const Camera& camera) override;
        void cleanup() override;
};



#endif //LIGHTINGPASS_H

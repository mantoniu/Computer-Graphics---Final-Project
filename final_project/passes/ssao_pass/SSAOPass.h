//
// Created by miche on 22/12/2024.
//

#ifndef SSAOPASS_H
#define SSAOPASS_H
#include <random>

#include "passes/geometry_pass/GeometryPass.h"
#include "passes/render_pass/RenderPass.h"


class SSAOPass : public RenderPass{
    GeometryPass &geometryPass;
    GLuint ssaoColorBuffer = 0;
    GLuint noiseTexture = 0;
    std::uniform_real_distribution<GLfloat> randomFloats = std::uniform_real_distribution<GLfloat> (0.0, 1.0);
    std::default_random_engine generator;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;

    public:
        SSAOPass(int width, int height, GeometryPass &geometryPass);

        void generateSampleKernel();
        void generateNoiseTexture();
        void loadNoiseTexture();

        void setup() override;
        void render(const std::vector<GraphicsObject*>& objects, const Camera& camera) override;
        void cleanup() override;

        [[nodiscard]] GLuint getColorBuffer() const;
};



#endif //SSAOPASS_H

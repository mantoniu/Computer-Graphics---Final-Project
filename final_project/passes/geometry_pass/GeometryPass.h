//
// Created by miche on 22/12/2024.
//

#ifndef GEOMETRYPASS_H
#define GEOMETRYPASS_H
#include "passes/render_pass/RenderPass.h"


class GeometryPass : public RenderPass {
    GLuint gPosition = 0;
    GLuint gPositionWorld = 0;
    GLuint gNormal = 0;
    GLuint gAlbedo = 0;
    GLuint gIgnoreLightingPass = 0;
    GLuint rboDepth = 0;

public:
    GeometryPass(int width, int height);

    void setup() override;

    void render(const std::vector<GraphicsObject *> &objects, const Camera &camera) override;

    void cleanup() override;

    [[nodiscard]] GLuint getGPosition() const;

    [[nodiscard]] GLuint getGPositionWorld() const;

    [[nodiscard]] GLuint getGNormal() const;

    [[nodiscard]] GLuint getGAlbedo() const;

    [[nodiscard]] GLuint getGIgnoreLightingPass() const;

    [[nodiscard]] GLuint getRboDepth() const;
};


#endif //GEOMETRYPASS_H

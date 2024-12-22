//
// Created by miche on 22/12/2024.
//

#ifndef SSAOBLURPASS_H
#define SSAOBLURPASS_H
#include "passes/render_pass/RenderPass.h"
#include "passes/ssao_pass/SSAOPass.h"


class SSAOBlurPass : public RenderPass{
    GLuint ssaoColorBufferBlur = 0;
    SSAOPass &ssaoPass;

    public:
        SSAOBlurPass(int width, int height, SSAOPass &ssaoPass);

        void setup() override;
        void render(const std::vector<GraphicsObject*>& objects, const Camera& camera) override;
        void cleanup() override;

        [[nodiscard]] GLuint getColorBufferBlur() const;
};



#endif //SSAOBLURPASS_H

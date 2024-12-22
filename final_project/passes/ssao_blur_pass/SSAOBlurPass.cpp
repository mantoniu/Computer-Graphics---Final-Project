//
// Created by miche on 22/12/2024.
//

#include "SSAOBlurPass.h"

#include <iostream>
#include <render/shader.h>

#include "utils/renderQuad.h"

SSAOBlurPass::SSAOBlurPass(int width, int height, SSAOPass &ssaoPass) : RenderPass(width, height, LoadShadersFromFile("../final_project/shaders/ssao.vert", "../final_project/shaders/ssao_blur.frag")), ssaoPass(ssaoPass){
}


void SSAOBlurPass::setup() {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());

    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getWidth(), getHeight(), 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;

    glUseProgram(getShaderID());
    glUniform1i(glGetUniformLocation(getShaderID(), "ssaoInput"), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOBlurPass::render(const std::vector<GraphicsObject *> &objects, const Camera &camera) {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(getShaderID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoPass.getColorBuffer());
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOBlurPass::cleanup() {
    if (ssaoColorBufferBlur!= 0) {
        glDeleteTextures(1, &ssaoColorBufferBlur);
        ssaoColorBufferBlur = 0;
    }
}

GLuint SSAOBlurPass::getColorBufferBlur() const {
    return ssaoColorBufferBlur;
}



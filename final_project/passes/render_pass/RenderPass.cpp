//
// Created by miche on 22/12/2024.
//

#include "RenderPass.h"

RenderPass::RenderPass(const int width, const int height, const GLuint shader) : shaderID(shader), width(width), height(height) {
    glGenFramebuffers(1, &FBO);
}

void RenderPass::cleanup() {
    if (FBO != 0) {
        glDeleteFramebuffers(1, &FBO);
    }
}

GLuint RenderPass::getFBO() const {
    return FBO;
}

int RenderPass::getWidth() const {
    return width;
}

int RenderPass::getHeight() const {
    return height;
}

GLuint RenderPass::getShaderID() const {
    return shaderID;
}

//
// Created by miche on 22/12/2024.
//

#include "DepthPass.h"

#include <render/shader.h>

DepthPass::DepthPass(const int width, const int height, std::vector<Light> &lights) : RenderPass(width, height,
        LoadShadersFromFile("../final_project/shaders/depth.vert", "../final_project/shaders/depth.frag")),
    lights(lights) {
}

void DepthPass::createDepthTextureArray() {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());

    // Create an array of texture for each lights
    glGenTextures(1, &depthTexturesArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexturesArray);

    // Allocation of memory for the array of depth textures
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, getWidth(), getHeight(), static_cast<int>(lights.size()),
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Setting up the array
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::render(const std::vector<GraphicsObject *> &objects, const Camera &camera) {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glViewport(0, 0, getWidth(), getHeight());

    for (int i = 0; i < lights.size(); i++) {
        glm::mat4 lightModelMatrix = lights[i].getSpaceMatrix();

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexturesArray, 0, i);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (const auto &object: objects) {
            glUseProgram(getShaderID());

            glm::mat4 mvp = lightModelMatrix * object->getModelMatrix();
            glUniformMatrix4fv(glGetUniformLocation(getShaderID(), "mvp"), 1, GL_FALSE, &mvp[0][0]);

            object->render(getShaderID());
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(0, 0, 1024, 768);
}

void DepthPass::setup() {
    createDepthTextureArray();
}

void DepthPass::cleanup() {
    if (depthTexturesArray != 0) {
        glDeleteTextures(1, &depthTexturesArray);
        depthTexturesArray = 0;
    }
    if (lightsUBO != 0) {
        glDeleteBuffers(1, &lightsUBO);
        lightsUBO = 0;
    }
}

GLuint DepthPass::getDepthTexturesArray() const {
    return depthTexturesArray;
}


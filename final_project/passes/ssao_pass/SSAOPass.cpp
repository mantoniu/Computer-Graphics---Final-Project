//
// Created by miche on 22/12/2024.
//

#include "SSAOPass.h"

#include <iostream>
#include <render/shader.h>

#include "utils/renderQuad.h"

SSAOPass::SSAOPass(const int width, const int height, GeometryPass &geometryPass) : RenderPass(width, height,
        LoadShadersFromFile("../final_project/shaders/ssao.vert", "../final_project/shaders/ssao.frag")),
    geometryPass(geometryPass) {
    generateSampleKernel();
    generateNoiseTexture();
}

void SSAOPass::generateSampleKernel() {
    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample = normalize(sample);
        sample *= randomFloats(generator);
        float scale = static_cast<float>(i) / 64.0f;

        scale = 0.1f + (scale * scale) * (0.9f);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
}

void SSAOPass::generateNoiseTexture() {
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
        // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
}

void SSAOPass::loadNoiseTexture() {
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAOPass::setup() {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());

    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getWidth(), getHeight(), 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    glUseProgram(getShaderID());
    glUniform1i(glGetUniformLocation(getShaderID(), "gPosition"), 0);
    glUniform1i(glGetUniformLocation(getShaderID(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(getShaderID(), "texNoise"), 2);

    loadNoiseTexture();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOPass::render(const std::vector<GraphicsObject *> &objects, const Camera &camera) {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(getShaderID());

    // Send kernel + rotation
    for (unsigned int i = 0; i < 64; ++i)
        glUniform3fv(glGetUniformLocation(getShaderID(), ("samples[" + std::to_string(i) + "]").c_str()), 1,
                     &ssaoKernel[i][0]);

    glm::mat4 projection = camera.getProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(getShaderID(), "projection"), 1, GL_FALSE, &projection[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGPosition());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGNormal());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOPass::cleanup() {
    if (ssaoColorBuffer != 0) {
        glDeleteRenderbuffers(1, &ssaoColorBuffer);
        ssaoColorBuffer = 0;
    }
    if (noiseTexture != 0) {
        glDeleteTextures(1, &noiseTexture);
        noiseTexture = 0;
    }
}

GLuint SSAOPass::getColorBuffer() const {
    return ssaoColorBuffer;
}



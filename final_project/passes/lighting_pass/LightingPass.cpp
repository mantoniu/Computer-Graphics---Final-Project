//
// Created by miche on 22/12/2024.
//

#include "LightingPass.h"

#include <render/shader.h>

#include "utils/renderQuad.h"

LightingPass::LightingPass(const int width, const int height, std::vector<Light *> &lights, GeometryPass &geometryPass,
                           SSAOBlurPass &ssaoBlurPass, DepthPass &depthPass) : RenderPass(width, height,
                                                                                   LoadShadersFromFile(
                                                                                       "../final_project/shaders/ssao.vert",
                                                                                       "../final_project/shaders/lighting.frag")),
                                                                               geometryPass(geometryPass),
                                                                               ssaoBlurPass(ssaoBlurPass),
                                                                               depthPass(depthPass), lights(lights) {
}

void LightingPass::setup() {
    glUseProgram(getShaderID());
    glUniform1i(glGetUniformLocation(getShaderID(), "gPosition"), 0);
    glUniform1i(glGetUniformLocation(getShaderID(), "gPositionWorld"), 1);
    glUniform1i(glGetUniformLocation(getShaderID(), "gNormal"), 2);
    glUniform1i(glGetUniformLocation(getShaderID(), "gAlbedo"), 3);
    glUniform1i(glGetUniformLocation(getShaderID(), "ssao"), 4);
    glUniform1i(glGetUniformLocation(getShaderID(), "gIgnoreLightingPass"), 5);
    glUniform1i(glGetUniformLocation(getShaderID(), "depthArray"), 6);
}

void LightingPass::loadLightsUBOs() {
    // Define the size of the UBO: two glm::vec3 (position + intensity) per light
    const size_t uboSize = sizeof(lightStruct) * lights.size();

    // Generate and bind the UBO
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<long long>(uboSize), nullptr, GL_DYNAMIC_DRAW);

    // Load light data into the UBO
    lightStruct uboData[lights.size()];
    for (int i = 0; i < lights.size(); i++) {
        uboData[i] = lights[i]->toStruct();
    }

    // Copy the data to the UBO
    glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<long long>(uboSize), &uboData);

    // Unbind the UBO
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    const GLuint blockIndex = glGetUniformBlockIndex(getShaderID(), "Lights");

    // Bind the UBO to the block index
    glUniformBlockBinding(getShaderID(), blockIndex, 0);

    // Bind the UBO to the binding point 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsUBO);
}

void LightingPass::render(const std::vector<GraphicsObject *> &objects, const Camera &camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(getShaderID());

    loadLightsUBOs();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGPosition());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGPositionWorld());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGNormal());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGAlbedo());
    glActiveTexture(GL_TEXTURE4); // add extra SSAO texture to lighting pass
    glBindTexture(GL_TEXTURE_2D, ssaoBlurPass.getColorBufferBlur());
    glActiveTexture(GL_TEXTURE5); // add extra SSAO texture to lighting pass
    glBindTexture(GL_TEXTURE_2D, geometryPass.getGIgnoreLightingPass());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthPass.getDepthTexturesArray());
    renderQuad();
}

void LightingPass::cleanup() {
    if (lightsUBO != 0) {
        glDeleteBuffers(1, &lightsUBO);
        lightsUBO = 0;
    }
}

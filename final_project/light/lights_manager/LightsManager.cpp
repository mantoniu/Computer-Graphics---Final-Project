//
// Created by miche on 07/12/2024.
//

#include "LightsManager.h"
#include <cstdarg>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "glad/gl.h"
#include "objects/graphics_object/GraphicsObject.h"

LightsManager::LightsManager(const std::vector<Light> &lights, const std::vector<GraphicsObject *>& objects) {
    this->lights = lights;
    this->objects = objects;

    loadLightsUBOs();
    createDepthTextureArray();
}

void LightsManager::render() {
    loadDepthTextures();
}

void LightsManager::createDepthTextureArray() {
    // Generate FBO
    glGenFramebuffers(1, &depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    // Create an array of texture for each lights
    glGenTextures(1, &depthTexturesArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexturesArray);

    // Allocation of memory for the array of depth textures
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, 2048, 2048, static_cast<int>(lights.size()), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Setting up the array
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void LightsManager::loadDepthTextures() {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glViewport(0, 0, 2048, 2048);

    for (int i=0; i<lights.size(); i++) {
        const glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(depthFoV), 1.0f, depthNear, depthFar);

        const glm::mat4 lightViewMatrix = lookAt(lights[i].getPosition(), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
        glm::mat4 lightViewProjection = lightProjectionMatrix * lightViewMatrix;

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexturesArray, 0, i);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (const auto &object: objects) {
            glUseProgram(object->getProgramId());

            object->render(lightViewProjection);

            if (const GLint location = glGetUniformLocation(object->getProgramId(), "depthArray"); location == -1) {
                std::cerr << "Error: uniform 'depthArray' not found in shader program." << std::endl;
            } else glUniform1i(location, 2);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexturesArray);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(0, 0, 1024, 768);
}

void LightsManager::loadLightsUBOs(){
    // Define the size of the UBO: two glm::vec3 (position + intensity) per light
    const size_t uboSize = sizeof(lightStruct)*lights.size();

    // Generate and bind the UBO
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, uboSize, nullptr, GL_STATIC_DRAW);

    // Load light data into the UBO
    lightStruct uboData[lights.size()];
    for (int i=0; i<lights.size(); i++){
        uboData[i] = lights[i].toStruct();
    }

    // Copy the data to the UBO
    glBufferSubData(GL_UNIFORM_BUFFER, 0, uboSize, &uboData);

    // Unbind the UBO
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Link the UBO to each shader program used by the objects
    for (const auto &object : objects) {
        // Get the uniform block index from the shader program
        const GLuint blockIndex = glGetUniformBlockIndex(object->getProgramId(), "Lights");

        // Bind the UBO to the block index
        glUniformBlockBinding(object->getProgramId(), blockIndex, 0);

        // Bind the UBO to the binding point 0
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsUBO);
    }
}

void LightsManager::clean() const {
    glDeleteFramebuffers(1, &depthFBO);
    glDeleteTextures(1, &depthTexturesArray);
    glDeleteBuffers(1, &lightsUBO);
}

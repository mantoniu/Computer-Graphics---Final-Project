//
// Created by miche on 02/12/2024.
//

#include "GraphicsObject.h"

#include <iostream>
#include <ostream>
#include <glm/gtc/matrix_transform.hpp>
#include <objects/cube/Cube.h>
#include <render/shader.h>

#include "light/lights_manager/LightsManager.h"

GraphicsObject::GraphicsObject(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    GLuint progID = LoadShadersFromFile(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if (progID == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return;
    }

    this->programId = progID;

    // Get a handle for GLSL variables
    mvpMatrixID = glGetUniformLocation(programId, "MVP");
    if (mvpMatrixID == -1) {
        std::cerr << "Uniform 'MVP' not found in shader." << std::endl;
    }
}


glm::mat4 GraphicsObject::getModelMatrix() const {
    auto model = glm::mat4(1.0f);
    model = translate(model, translation);
    model = rotate(model, glm::radians(rotation), rotationAxis);
    model = glm::scale(model, scale);
    return model;
}

void GraphicsObject::setScale(const glm::vec3 scale) {
    this->scale = scale;
}

void GraphicsObject::setTranslation(const glm::vec3 translation) {
    this->translation = translation;
}

void GraphicsObject::setRotation(const float rotation, const glm::vec3 rotationAxis) {
    this->rotation = rotation;
    this->rotationAxis = rotationAxis;
}

void GraphicsObject::render(glm::mat4 &cameraMatrix) {
    glUseProgram(programId);

    // Load the MVP matrix for the shader
    glm::mat4 mvp = cameraMatrix * getModelMatrix();
    glUniformMatrix4fv(static_cast<int>(mvpMatrixID), 1, GL_FALSE, &mvp[0][0]);
}

void GraphicsObject::cleanup() {
    glDeleteProgram(programId);
}

GLuint GraphicsObject::getProgramId() const {
    return programId;
}

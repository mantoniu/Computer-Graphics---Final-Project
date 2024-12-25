//
// Created by miche on 02/12/2024.
//

#include "GraphicsObject.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <3D_objects/cube/Cube.h>
#include <render/shader.h>

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

void GraphicsObject::render(const GLuint programID) {
    glUseProgram(programID);

    modelMatrixID = glGetUniformLocation(programID, "model");
    glUniformMatrix4fv(static_cast<int>(modelMatrixID), 1, GL_FALSE, &getModelMatrix()[0][0]);
}

void GraphicsObject::cleanup() {
    glDeleteTextures(1, &modelMatrixID);
}
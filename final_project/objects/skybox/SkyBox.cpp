//
// Created by miche on 27/11/2024.
//

#include "SkyBox.h"

#include <iostream>
#include <ostream>
#include <glm/detail/func_common.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>

#include "../../texture_utils/texture_utils.h"

SkyBox::SkyBox() : Cube(glm::vec3(0), glm::vec3(100), default_vertex_buffer_data, default_color_buffer_data, default_normal_buffer_data, skybox_index_buffer_data){
    // Enable UV buffer and texture
    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(skybox_uv_buffer_data.size()*sizeof(GLfloat)), skybox_uv_buffer_data.data(), GL_STATIC_DRAW);

    // Loading the texture
    std::string texturePath = "../final_project/objects/skybox/sky.png";
    std::cout << "Texture path: " << texturePath << std::endl;
    textureID = LoadTextureTileBox(texturePath.c_str());

    textureSamplerID = glGetUniformLocation(getProgramID(), "textureSampler");
}

void SkyBox::loadBuffers() {
    Cube::loadBuffers();

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Set textureSampler to user texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(static_cast<int>(textureSamplerID), 0);
}

void SkyBox::disableVertexAttribArrays() {
    Cube::disableVertexAttribArrays();
    glDisableVertexAttribArray(3);
}

void SkyBox::cleanup() {
    Cube::cleanup();
    glDeleteBuffers(1, &uvBufferID);
    glDeleteTextures(1, &textureID);
}
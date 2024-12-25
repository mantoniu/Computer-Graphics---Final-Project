//
// Created by miche on 27/11/2024.
//

#include "SkyBox.h"

#include <iostream>
#include <ostream>
#include <render/shader.h>

#include "../../utils/texture_utils.h"

SkyBox::SkyBox() : Cube(default_vertex_buffer_data, default_color_buffer_data, default_normal_buffer_data, skybox_index_buffer_data){
    setScale(glm::vec3(100.0));

    // Enable UV buffer and texture
    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(skybox_uv_buffer_data.size()*sizeof(GLfloat)), skybox_uv_buffer_data.data(), GL_STATIC_DRAW);

    // Loading the texture
    std::string texturePath = "../final_project/3D_objects/skybox/skybox.png";
    std::cout << "Texture path: " << texturePath << std::endl;
    textureID = LoadTextureTileBox(texturePath.c_str());
}

void SkyBox::loadBuffers(const GLuint programID) {
    Cube::loadBuffers(programID);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glUniform1i(glGetUniformLocation(programID, "ignoreLightingPass"), 1);

    // Set textureSampler to user texture unit 0
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureID);
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
    glUniform1i(static_cast<int>(textureSamplerID), 3);
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
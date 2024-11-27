//
// Created by Student on 06/11/2024.
//

#include "Cube.h"

#include <iostream>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>

Cube::Cube() : Cube(glm::vec3(0.0f), glm::vec3(10.0f)){
}

Cube::Cube(const glm::vec3 position, const glm::vec3 scale) : Cube(position, scale, default_vertex_buffer_data, default_color_buffer_data, default_normal_buffer_data, default_index_buffer_data){
}

Cube::Cube(const glm::vec3 position, const glm::vec3 scale, GLuint programId) : Cube(position, scale, default_vertex_buffer_data, default_color_buffer_data, default_normal_buffer_data, default_index_buffer_data, programId) {
}

Cube::Cube(const glm::vec3 position, const glm::vec3 scale, const std::vector<GLfloat> &vertex_buffer_data, const std::vector<GLfloat> &color_buffer_data, const std::vector<GLfloat> &normal_buffer_data, const std::vector<GLuint> &index_buffer_data) : Cube(position, scale, vertex_buffer_data, color_buffer_data, normal_buffer_data, index_buffer_data, LoadShadersFromFile("../final_project/objects/shaders/box.vert", "../final_project/objects/shaders/box.frag")){
}

Cube::Cube(const glm::vec3 position, const glm::vec3 scale, const std::vector<GLfloat> &vertex_buffer_data, const std::vector<GLfloat> &color_buffer_data, const std::vector<GLfloat> &normal_buffer_data, const std::vector<GLuint> &index_buffer_data, GLuint programID) {
    this->position = position;
    this->scale = scale;
    this->programID = programID;

    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create a vertex buffer object to store the vertex data
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertex_buffer_data.size()*sizeof(GLfloat)), vertex_buffer_data.data(), GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(color_buffer_data.size()*sizeof(GLfloat)), color_buffer_data.data(), GL_STATIC_DRAW);

    // Create a vertex buffer object to store the vertex normals
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(normal_buffer_data.size()*sizeof(GLfloat)), normal_buffer_data.data(), GL_STATIC_DRAW);

    // Create an index buffer object to store the index data that defines triangle faces
    index_data_size = static_cast<int>(index_buffer_data.size());
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  static_cast<GLsizeiptr>(normal_buffer_data.size()*sizeof(GLuint)), index_buffer_data.data(), GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shader
    mvpMatrixID = glGetUniformLocation(this->programID, "MVP");
}

GLuint Cube::getProgramID() const {
    return programID;
}

void Cube::render(const glm::mat4 &cameraMatrix) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glm::mat4 mvp = cameraMatrix;

    if(!glm::isnan(scale.x)) {
        auto modelMatrix = glm::mat4();

        // Translate the box to its position
        modelMatrix = translate(modelMatrix, position);

        // Scale the box along each axis
        modelMatrix = glm::scale(modelMatrix, scale);

        mvp *= modelMatrix;
    }

    // ------------------------------------
    glUniformMatrix4fv(static_cast<int>(mvpMatrixID), 1, GL_FALSE, &mvp[0][0]);

    // Draw the box
    glDrawElements(
        GL_TRIANGLES,      // mode
        index_data_size,   // number of indices
        GL_UNSIGNED_INT,   // type
        static_cast<void *>(nullptr) // element array buffer offset (void*) 0
                   // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Cube::cleanup(){
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
}
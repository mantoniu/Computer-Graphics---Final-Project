//
// Created by Student on 06/11/2024.
//

#include "Cube.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>


Cube::Cube() : Cube(default_vertex_buffer_data, default_color_buffer_data, default_normal_buffer_data, default_index_buffer_data){
}

Cube::Cube(const std::vector<GLfloat> &vertex_buffer_data, const std::vector<GLfloat> &color_buffer_data,
           const std::vector<GLfloat> &normal_buffer_data, const std::vector<GLuint> &index_buffer_data) {
    this->position = position;
    this->scale = scale;

    // Create a vao object
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

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

    glBindVertexArray(0);
}

void Cube::render(const GLuint programID) {
    GraphicsObject::render(programID);

    glBindVertexArray(vaoID);

    loadBuffers(programID);

    // Draw the box
    glDrawElements(
        GL_TRIANGLES,      // mode
        index_data_size,   // number of indices
        GL_UNSIGNED_INT,   // type
        static_cast<void *>(nullptr) // element array buffer offset (void*) 0
                   // element array buffer offset
    );

    disableVertexAttribArrays();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}


void Cube::disableVertexAttribArrays() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Cube::loadBuffers(GLuint programID) {
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
}

void Cube::cleanup(){
    GraphicsObject::cleanup();
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
}

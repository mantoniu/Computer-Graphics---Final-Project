//
// Created by miche on 27/11/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H
#include "../cube/Cube.h"

inline const std::vector<GLfloat> skybox_uv_buffer_data = {
    // Front
    0.5f, 2.0f/3.0f,
    0.25f, 2.0f/3.0f,
    0.25f, 1.0f/3.0f,
    0.5f, 1.0f/3.0f,

    // Back
    1.0f, 2.0f/3.0f,
    0.75f, 2.0f/3.0f,
    0.75f, 1.0f/3.0f,
    1.0f, 1.0f/3.0f,

    // Right
    0.75f, 2.0f/3.0f,
    0.5f, 2.0f/3.0f,
    0.5f, 1.0f/3.0f,
    0.75f, 1.0f/3.0f,

    // Left
    0.25f, 2.0f/3.0f,
    0.0f, 2.0f/3.0f,
    0.0f, 1.0f/3.0f,
    0.25f, 1.0f/3.0f,

    // Top - we do not want texture the top
    0.5f, 1.0f/3.0f,
    0.25f, 1.0f/3.0f,
    0.25f, 0.0f,
    0.5f, 0.0f,

    // Bottom - we do not want texture the top
    0.5f, 1.0f,
    0.25f, 1.0f,
    0.25f, 2.0f/3.0f,
    0.5f, 2.0f/3.0f,
};

inline const std::vector<GLuint> skybox_index_buffer_data = {
    0, 3, 2,
    0, 2, 1,

    4, 7, 6,
    4, 6, 5,

    8, 11, 10,
    8, 10, 9,

    12, 15, 14,
    12, 14, 13,

    16, 19, 18,
    16, 18, 17,

    20, 23, 22,
    20, 22, 21
};

class SkyBox : public Cube{
    GLuint uvBufferID;
    GLuint textureID;
    GLuint textureSamplerID;

    public:
        SkyBox();
        void disableVertexAttribArrays() override;
        void loadBuffers() override;
        void cleanup() override;
};



#endif //SKYBOX_H

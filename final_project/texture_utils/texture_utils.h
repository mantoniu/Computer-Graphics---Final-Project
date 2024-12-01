//
// Created by miche on 27/11/2024.
//

#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H
#include <tiny_gltf.h>

#include "glad/gl.h"

GLuint LoadTextureTileBox(const char *texture_file_path);
GLuint LoadTexture(const tinygltf::Image& gltfImage);

#endif //TEXTURE_UTILS_H

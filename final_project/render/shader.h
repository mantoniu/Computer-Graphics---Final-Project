#ifndef SHADER_H_
#define SHADER_H_

#include <glad/gl.h>
#include <string>

GLuint LoadShadersFromFile(const char *vertex_file_path, const char *fragment_file_path);

GLuint LoadShadersFromString(const std::string& VertexShaderCode, const std::string &FragmentShaderCode);

#endif

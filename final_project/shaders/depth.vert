#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;

uniform mat4 mvp;
uniform int animated;
uniform mat4 jointMatrices[100];

void main() {
    mat4 finalMatrix;
    if (animated == 1) {
        mat4 skinMatrix =
        a_weight.x * jointMatrices[int(a_joint.x)] +
        a_weight.y * jointMatrices[int(a_joint.y)] +
        a_weight.z * jointMatrices[int(a_joint.z)] +
        a_weight.w * jointMatrices[int(a_joint.w)];
        finalMatrix = skinMatrix;
    } else {
        finalMatrix = mat4(1.0);
    }
    gl_Position = mvp * finalMatrix * vec4(vertexPosition, 1.0);
}
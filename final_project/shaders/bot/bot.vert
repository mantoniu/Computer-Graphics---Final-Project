#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;
out vec4 debugColor;


uniform mat4 MVP;
uniform mat4 jointMatrices[100];

void main() {
    mat4 skinMatrix =
    a_weight.x * jointMatrices[int(a_joint.x)] +
    a_weight.y * jointMatrices[int(a_joint.y)] +
    a_weight.z * jointMatrices[int(a_joint.z)] +
    a_weight.w * jointMatrices[int(a_joint.w)];

    debugColor = vec4(a_weight.x, a_weight.y, a_weight.z, 1.0);
    // Transform vertex
    gl_Position =  MVP * skinMatrix * vec4(vertexPosition, 1.0);

    // World-space geometry
    worldPosition = vec3(skinMatrix * vec4(vertexPosition, 1.0));
    worldNormal = mat3(skinMatrix) * vertexNormal;
}

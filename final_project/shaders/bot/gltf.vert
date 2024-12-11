#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;
layout(location = 5) in vec4 m_color;

// Output data, to be interpolated for each fragment
out vec4 worldPosition;
out vec3 worldNormal;
out vec2 texCoord;
out vec4 color;
flat out int texIndex;
flat out int metTexIndex;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 jointMatrices[100];
uniform int colorTextureIndex;
uniform int metTextureIndex;
uniform int animated;

void main() {
    // Compute skin matrix only if animated
    mat4 finalMatrix;
    if (animated == 1) {
        mat4 skinMatrix =
        a_weight.x * jointMatrices[int(a_joint.x)] +
        a_weight.y * jointMatrices[int(a_joint.y)] +
        a_weight.z * jointMatrices[int(a_joint.z)] +
        a_weight.w * jointMatrices[int(a_joint.w)];

        finalMatrix = skinMatrix;
    } else {
        // Use identity matrix if not animated
        finalMatrix = mat4(1.0);
    }

    // Transform vertex
    gl_Position = MVP * finalMatrix * vec4(vertexPosition, 1.0);

    // World-space geometry
    worldPosition = modelMatrix * finalMatrix * vec4(vertexPosition, 1.0);
    worldNormal = mat3(finalMatrix) * vertexNormal;

    // Pass texture coordinates
    texCoord = vertexUV;
    texIndex = colorTextureIndex;
    metTexIndex = metTextureIndex;
    color = m_color;
}
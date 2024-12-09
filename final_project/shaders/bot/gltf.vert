#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition; // Position du vertex
layout(location = 1) in vec3 vertexNormal;   // Normale du vertex
layout(location = 2) in vec2 vertexUV;       // Coordonnées de texture
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;
layout(location = 5) in vec4 m_color;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;  // Position du vertex dans l'espace du monde
out vec3 worldNormal;    // Normale dans l'espace du monde
out vec2 texCoord;       // Coordonnées de texture pour le fragment
out vec4 color;
flat out int texIndex;
flat out int metTexIndex;

uniform mat4 MVP;  // Matrice de transformation Model-View-Projection
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
    worldPosition = vec3(finalMatrix * vec4(vertexPosition, 1.0));
    worldNormal = mat3(finalMatrix) * vertexNormal;

    // Pass texture coordinates
    texCoord = vertexUV;
    texIndex = colorTextureIndex;
    metTexIndex = metTextureIndex;
    color = m_color;
}
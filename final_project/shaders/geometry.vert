#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;
layout(location = 5) in vec4 m_color;

out vec3 FragPos;
out vec3 FragPosWorld;
out vec2 TexCoords;
out vec3 Normal;
out vec4 color;
flat out int texIndex;
flat out int metTexIndex;

uniform bool invertedNormals;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int colorTextureIndex;
uniform int metTextureIndex;
uniform int animated;

uniform mat4 jointMatrices[100];

void main()
{
    mat4 finalMatrix = mat4(1.0);

    if (animated == 1) {
        finalMatrix =
        a_weight.x * jointMatrices[int(a_joint.x)] +
        a_weight.y * jointMatrices[int(a_joint.y)] +
        a_weight.z * jointMatrices[int(a_joint.z)] +
        a_weight.w * jointMatrices[int(a_joint.w)];
    }

    // Position en espace vue
    vec4 viewPos = view * model * finalMatrix * vec4(vertexPosition, 1.0);
    FragPos = viewPos.xyz;
    vec4 worldPosition = model * finalMatrix * vec4(vertexPosition, 1.0);
    FragPosWorld = vec3(worldPosition);

    TexCoords = vertexUV;

    // Normales en espace vue
    mat3 normalMatrix = transpose(inverse(mat3(view * model * finalMatrix)));
    Normal = normalize(normalMatrix * (invertedNormals ? -vertexNormal : vertexNormal));

    gl_Position = projection * viewPos;

    texIndex = colorTextureIndex;
    metTexIndex = metTextureIndex;
    color = m_color;
}
#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gPositionWorld;
layout (location = 4) out float gIgnoreLightingPass;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 FragPosWorld;
in vec3 Normal;
in vec4 color;
flat in int texIndex;
flat in int metTexIndex;

uniform int ignoreLightingPass;
uniform vec4 baseColorFactor;
uniform sampler2DArray textureArray;
uniform sampler2DArray metTextureArray;
uniform sampler2DArray depthArray;
uniform sampler2D textureSampler;

void main()
{
    gPosition = FragPos;
    gPositionWorld = FragPosWorld;

    gNormal = normalize(Normal);

    vec4 baseColor = ((texIndex!=-1) ? texture(textureArray, vec3(TexCoords, texIndex)) : color);
    baseColor *= baseColorFactor;
    gIgnoreLightingPass = ignoreLightingPass;

    if (ignoreLightingPass==1)
    gAlbedo.rgb = (texture(textureSampler, TexCoords)).rgb;
    else gAlbedo.rgb = baseColor.rgb;
}
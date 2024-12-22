#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gPositionWorld;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
uniform sampler2D gIgnoreLightingPass;
uniform sampler2DArray depthArray;

float specularStrength = 0.3;
float shininess = 15.0;

struct structLight {
    vec3 position;
    float intensity;
    vec3 color;
    mat4 spaceMatrix;
};

layout(std140) uniform Lights {
    structLight lights[10];
};

float shadowCalculation(vec4 fragPosLightSpace, int i) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec3 UVCoords = projCoords * 0.5 + 0.5;

    float existingDepth = texture(depthArray, vec3(UVCoords.xy, i)).x;
    float currentDepth = UVCoords.z;
    float bias = 1e-3;

    float shadow = fragPosLightSpace.z >= 0 && currentDepth >= existingDepth + bias ? 0.2 : 1.0;
    return shadow;
}

vec3 computePhongLighting(vec3 fragPos, vec3 worldPosition, vec3 normal, vec3 diffuse, float ao) {
    vec3 accumulatedLighting = vec3(0.0);
    vec3 viewDir = normalize(-fragPos);

    for (int i = 0; i < 10; i++) {
        vec3 lightPosition = lights[i].position;
        float lightIntensity = lights[i].intensity;
        vec3 lightColor = lights[i].color;
        vec4 fragPosLightSpace = lights[i].spaceMatrix * vec4(worldPosition, 1.0);

        vec3 lightDir = lightPosition - worldPosition;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);

        float shadowFactor = shadowCalculation(fragPosLightSpace, i);

        float NdotL = max(dot(normal, lightDir), 0.0);
        vec3 diffuseLight = diffuse * NdotL * lightColor;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        vec3 specularLight = specularStrength * spec * lightColor;

        float attenuation = lightIntensity / (distance * distance);

        vec3 lightContribution = shadowFactor * attenuation * (diffuseLight + specularLight);
        lightContribution *= mix(1.0, ao, 0.5);

        accumulatedLighting += lightContribution;
    }
    return accumulatedLighting;
}

void main() {
    float ignoreLightingPass = texture(gIgnoreLightingPass, TexCoords).r;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;

    if (ignoreLightingPass==1){
        FragColor = vec4(Diffuse, 1.0);
        return;
    }

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 worldPosition = texture(gPositionWorld, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    vec3 ambient = 0.3 * Diffuse * AmbientOcclusion;

    vec3 phongLighting = computePhongLighting(FragPos, worldPosition, Normal, Diffuse, AmbientOcclusion);

    vec3 lighting = ambient + phongLighting;

    lighting = pow(lighting, vec3(1.0/2.2));

    FragColor = vec4(lighting, 1.0);
}
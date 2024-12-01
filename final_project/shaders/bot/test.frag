#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
flat in int texIndex;

// Uniforms pour les textures (sampler arrays)
uniform sampler2DArray textureArray;

// Uniform Buffer Object pour les matériaux
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

out vec3 finalColor;

void main() {
    // Lighting
    vec3 lightDir = lightPosition - worldPosition;
    float lightDist = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);
    vec3 v = lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0) / lightDist;

    // Texture sampling
    vec4 baseColor = texture(textureArray, vec3(texCoord, texIndex));

    vec3 color = baseColor.rgb * v;

    // Tone mapping
    color = color / (1.0 + color);

    // Gamma correction
    finalColor = pow(color, vec3(1.0 / 2.2));
}

#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
flat in int texIndex;
flat in int metTexIndex;

uniform sampler2DArray textureArray;
uniform sampler2DArray metTextureArray;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 viewPosition;

out vec3 finalColor;

void main() {
    // Default base color when no texture is present
    vec4 baseColor = vec4(1.0);

    // Check if base color texture is available
    if (texIndex >= 0) {
        baseColor = texture(textureArray, vec3(texCoord, texIndex));
    }

    // Default metallic and roughness values when no texture is present
    float metallic = 0.0;
    float roughness = 0.5;

    // Check if metallic-roughness texture is available
    if (metTexIndex >= 0) {
        vec4 metData = texture(metTextureArray, vec3(texCoord, metTexIndex));
        metallic = metData.b;
        roughness = metData.g;
    }

    // Calculate light and view directions
    vec3 viewDir = normalize(viewPosition - worldPosition);
    vec3 lightDir = normalize(lightPosition - worldPosition);
    float lightDist = length(lightPosition - worldPosition);

    // Normalize the surface normal
    vec3 N = normalize(worldNormal);

    // Compute diffuse component with reduced intensity
    float diffuseFactor = max(dot(N, lightDir), 0.0);
    float attenuation = 1.0 / (1.0 + lightDist * 2.0);
    vec3 diffuse = baseColor.rgb * diffuseFactor * lightIntensity * attenuation * 0.3 * (1.0 - metallic);

    // Compute specular component with minimal contribution
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specFactor = pow(max(dot(N, halfwayDir), 0.0), 4.0 * (1.0 - roughness));
    vec3 specColor = mix(vec3(0.04), baseColor.rgb, metallic);
    vec3 specular = specColor * specFactor * lightIntensity * attenuation * 0.1;

    // Add a very subtle ambient term
    vec3 ambient = baseColor.rgb * 0.02;

    // Combine all lighting components
    vec3 color = ambient + diffuse + specular;

    // Apply tone mapping
    color = color / (color + 1.0);

    // Apply gamma correction
    finalColor = pow(color, vec3(1.0 / 2.2));
}
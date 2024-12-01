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

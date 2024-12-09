#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
in vec4 color;
flat in int texIndex;
flat in int metTexIndex;

uniform vec4 baseColorFactor;
uniform sampler2DArray textureArray;
uniform sampler2DArray metTextureArray;

struct structLight{
	vec3 intensity;
	vec3 position;
};

layout(std140) uniform Lights {
	structLight lights[10];
};

out vec4 finalColor;

void main() {
	// Lighting
	vec3 accumulatedLighting = vec3(0.0);

	for(int i=0; i<10; i++){
		vec3 lightPosition = lights[i].position;
		vec3 lightIntensity = lights[i].intensity;

		vec3 lightDir = lightPosition - worldPosition; // Direction de la lumière
		float lightDist = dot(lightDir, lightDir);     // Distance au carré
		lightDir = normalize(lightDir);               // Normalisation de la direction

		// Compute the light contribution
		vec3 lightContribution = lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0) / lightDist;

		// Accumulate the light contribution
		accumulatedLighting += lightContribution;
	}

	// Texture sampling
	vec4 baseColor = ((texIndex!=-1) ? texture(textureArray, vec3(texCoord, texIndex)) : color);
	baseColor *= baseColorFactor;

	vec3 tcolor = baseColor.rgb * accumulatedLighting;

	// Tone mapping
	tcolor = tcolor / (1.0 + tcolor);

	// Gamma correction
	finalColor = pow(vec4(tcolor, 1.0f), vec4(1.0 / 2.2));
}

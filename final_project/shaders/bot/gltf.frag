#version 330 core

in vec4 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
in vec4 color;
flat in int texIndex;
flat in int metTexIndex;

uniform vec4 baseColorFactor;
uniform sampler2DArray textureArray;
uniform sampler2DArray metTextureArray;
uniform sampler2DArray depthArray;

struct structLight{
	vec3 position;
	float intensity;
	vec3 color;
	mat4 spaceMatrix;
};

layout(std140) uniform Lights {
	structLight lights[10];
};

out vec4 finalColor;

float shadowCalculation(vec4 fragPosLightSpace, int i) {
	// Convert light-space position to NDC
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	vec3 UVCoords = projCoords * 0.5 + 0.5;

	// Retrieve the closest depth from the shadow map at this fragment’s location
	float existingDepth = texture(depthArray, vec3(UVCoords.xy, i)).x;

	// The current fragment’s depth in light space
	float currentDepth = UVCoords.z;

	// Small bias to reduce shadow acne (self-shadowing artifacts)
	float bias = 1e-3;

	// Determine shadow: if currentDepth is greater than the existing depth, the fragment is in shadow
	float shadow = fragPosLightSpace.z >= 0 && currentDepth >= existingDepth + bias ? 0.2 : 1.0;

	return shadow;
}

vec3 computeAccumulatedLighting(){
	vec3 accumulatedLighting = vec3(0.0);

	for(int i=0; i<10; i++){

		vec3 lightPosition = lights[i].position;
		float lightIntensity = lights[i].intensity;
		vec3 lightColor = lights[i].color;
		vec4 fragPosLightSpace = lights[i].spaceMatrix * worldPosition;

		vec3 lightDir = lightPosition - worldPosition.xyz;
		float lightDist = dot(lightDir, lightDir);
		lightDir = normalize(lightDir);

		// Calculate the shadow factor for this light
		float shadowFactor = shadowCalculation(fragPosLightSpace, i);

		// Compute the light contribution
		vec3 lightContribution = shadowFactor * lightIntensity * lightColor * clamp(dot(lightDir, worldNormal), 0.0, 1.0) / lightDist;

		// Accumulate the light contribution
		accumulatedLighting += lightContribution;
	}
	return accumulatedLighting;
}

void main() {
	// Lighting
	vec3 accumulatedLighting = computeAccumulatedLighting();

	// Texture sampling
	vec4 baseColor = ((texIndex!=-1) ? texture(textureArray, vec3(texCoord, texIndex)) : color);
	baseColor *= baseColorFactor;

	vec3 tcolor = baseColor.rgb * accumulatedLighting;

	// Tone mapping
	tcolor = tcolor / (1.0 + tcolor);

	// Gamma correction
	finalColor = pow(vec4(tcolor, 1.0f), vec4(1.0 / 2.2));
}

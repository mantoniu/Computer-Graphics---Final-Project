//
// Created by miche on 29/11/2024.
//

#ifndef GLTFOBJECT_H
#define GLTFOBJECT_H
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>

#include "glad/gl.h"
#include "light/Light.h"
#include <tiny_gltf.h>
#include "objects/graphics_object/GraphicsObject.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Texture types
struct Material {
	glm::vec4 baseColorFactor;
	glm::vec3 emissiveFactor;
	float metallicFactor;
	float roughnessFactor;
	float occlusionFactor;
	int hasBaseColorTexture;
	int hasMetallicRoughnessTexture;
	int hasNormalTexture;
	int hasEmissiveTexture;
	int hasOcclusionTexture;
};

// Each VAO corresponds to each mesh primitive in the GLTF graphics_object
struct PrimitiveObject {
	GLuint vao;
	std::map<int, GLuint> vbos;
};

// Skinning
struct SkinObject {
	// Transforms the geometry into the space of the respective joint
	std::vector<glm::mat4> inverseBindMatrices;

	// Transforms the geometry following the movement of the joints
	std::vector<glm::mat4> globalJointTransforms;

	// Combined transforms
	std::vector<glm::mat4> jointMatrices;
};

// Animation
struct SamplerObject {
	std::vector<float> input;
	std::vector<glm::vec4> output;
	int interpolation;
};

struct ChannelObject {
	int sampler;
	std::string targetPath;
	int targetNode;
};

struct AnimationObject {
	std::vector<SamplerObject> samplers;	// Animation data
};

class GltfObject : public GraphicsObject{
	private:
		// Shader variable IDs
		GLuint mvpMatrixID;
		GLuint jointMatricesID;
		GLuint lightPositionID;
		GLuint lightIntensityID;
		GLuint programID;

		tinygltf::Model model;

		std::vector<PrimitiveObject> primitiveObjects;
		std::vector<SkinObject> skinObjects;
		std::vector<AnimationObject> animationObjects;

		int animated;

		// base color
		std::map<int, int> baseColorMaterialIndices;
		std::vector<int> baseColorTexturesIndices;
		int colorTexturesCount = 0;

		// metallic roughness
		std::map<int, int> metallicRoughnessMaterialIndices;
		std::vector<int> metallicRoughnessTexturesIndices;
	    int metaliicRoughnessTextureCount = 0;

		// Material array
		Material materialsData[100];

		// Textures arrays
		GLuint colorTexturesID;
		GLuint normalTexturesID;
		GLuint emissiveTexturesID;
		GLuint occlusionTexturesID;
		GLuint metallicRoughnessTextureID;

		GLuint uboMaterials;

	public:
		explicit GltfObject(const std::string& filePath);

		GltfObject(const std::string &filePath, bool animated);

		static glm::mat4 getNodeTransform(const tinygltf::Node& node);
		static void computeLocalNodeTransform(const tinygltf::Model& model, int nodeIndex, std::vector<glm::mat4> &localTransforms);
		static void computeGlobalNodeTransform(const tinygltf::Model& model, const std::vector<glm::mat4> &localTransforms, int nodeIndex, const glm::mat4& parentTransform, std::vector<glm::mat4> &globalTransforms);

		static std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model);

		static std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model);
		static void updateAnimation(const tinygltf::Model &model, const tinygltf::Animation &anim, const AnimationObject &animationObject, float time, std::vector<glm::mat4> &nodeTransforms);
		void updateSkinning(const std::vector<glm::mat4> &nodeTransforms);
		void update(float time);

		static bool loadModel(tinygltf::Model &model, const char *filename);

		void bindTexture(const std::string &textureType, const tinygltf::Model &model, const tinygltf::Primitive &primitive) const;

		void bindMesh(std::vector<PrimitiveObject>& primitiveObjects, const tinygltf::Model& model, tinygltf::Mesh& mesh);
		void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, tinygltf::Model &model, tinygltf::Node &node);
		std::vector<PrimitiveObject> bindModel(tinygltf::Model &model);

		GLuint initTextureArrays(std::vector<int> textureIndices) const;

		void loadAndResizeTexture(int textureIndex, int width, int height);

		void preloadTextures();

		GLuint loadTexture(int textureIndex) const;
		void loadMaterials();

		void initBuffers();

		void uploadMaterialData();

		void bindTexture(const tinygltf::Model &model, const tinygltf::Primitive &primitive);

		void bindTextures();

		void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects, const tinygltf::Model &model,
		              const tinygltf::Mesh &mesh);

		void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model &model, const tinygltf::Node &node);
		void drawModel(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model &model);

		void render(glm::mat4 &cameraMatrix, Light light) override;
	    void cleanup() override;
};

#endif //GLTFOBJECT_H

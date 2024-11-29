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

	public:
		explicit GltfObject(const std::string& filePath);
		static glm::mat4 getNodeTransform(const tinygltf::Node& node);
		static void computeLocalNodeTransform(const tinygltf::Model& model, int nodeIndex, std::vector<glm::mat4> &localTransforms);
		static void computeGlobalNodeTransform(const tinygltf::Model& model, const std::vector<glm::mat4> &localTransforms, int nodeIndex, const glm::mat4& parentTransform, std::vector<glm::mat4> &globalTransforms);

		static std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model);

		static std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model);
		static void updateAnimation(const tinygltf::Model &model, const tinygltf::Animation &anim, const AnimationObject &animationObject, float time, std::vector<glm::mat4> &nodeTransforms);
		void updateSkinning(const std::vector<glm::mat4> &nodeTransforms);
		void update(float time);

		static bool loadModel(tinygltf::Model &model, const char *filename);

		static void bindMesh(std::vector<PrimitiveObject> &primitiveObjects, const tinygltf::Model &model, tinygltf::Mesh &mesh);
		static void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, tinygltf::Model &model, tinygltf::Node &node);
		static std::vector<PrimitiveObject> bindModel(tinygltf::Model &model);

		static void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model &model, const tinygltf::Node &node);
		static void drawModel(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model &model);

		void render(glm::mat4 &cameraMatrix, Light light) override;
	    void cleanup() override;
};

#endif //GLTFOBJECT_H

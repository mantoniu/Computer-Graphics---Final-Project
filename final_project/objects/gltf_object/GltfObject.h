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
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Each VAO corresponds to each mesh primitive in the GLTF model
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

class GltfObject {
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
		GltfObject(std::string filePath);
		glm::mat4 getNodeTransform(const tinygltf::Node& node);
		void computeLocalNodeTransform(const tinygltf::Model& model, int nodeIndex, std::vector<glm::mat4> &localTransforms);
		void computeGlobalNodeTransform(const tinygltf::Model& model, const std::vector<glm::mat4> &localTransforms, int nodeIndex, const glm::mat4& parentTransform, std::vector<glm::mat4> &globalTransforms);

		std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model);

		static std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model);
		void updateAnimation(const tinygltf::Model &model, const tinygltf::Animation &anim, const AnimationObject &animationObject, float time, std::vector<glm::mat4> &nodeTransforms);
		void updateSkinning(const std::vector<glm::mat4> &nodeTransforms);
		void update(float time);

		bool loadModel(tinygltf::Model &model, const char *filename);

		void bindMesh(std::vector<PrimitiveObject> &primitiveObjects, tinygltf::Model &model, tinygltf::Mesh &mesh);
		void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, tinygltf::Model &model, tinygltf::Node &node);
		std::vector<PrimitiveObject> bindModel(tinygltf::Model &model);

		void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model &model, tinygltf::Node &node);
		void drawModel(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model &model);

		void render(glm::mat4 cameraMatrix, Light light);
	    void cleanup();
};

#endif //GLTFOBJECT_H

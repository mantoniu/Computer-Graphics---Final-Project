//
// Created by miche on 29/11/2024.
//

#include "GltfObject.h"

#include <cmath>
#include <iostream>
#include <stb_image.h>
#include <stb_image_resize.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <objects/cube/Cube.h>
#include <render/shader.h>
#include <utils/texture_utils.h>
#include "light/Light.h"

GltfObject::GltfObject(const std::string& filePath) : GltfObject(filePath, false){}

GltfObject::GltfObject(const std::string& filePath, bool animated) : GraphicsObject(){
	// Modify your path if needed
	if (!loadModel(model, filePath.c_str())) {
		return;
	}

	this->animated = animated;

	// Prepare buffers for rendering
	primitiveObjects = bindModel(model);

	// Prepare joint matrices
	if (animated)
		skinObjects = prepareSkinning(model);

	// Prepare animation data
	animationObjects = prepareAnimation(model);

	loadMaterials();

	colorTexturesID = initTextureArrays(baseColorTexturesIndices);
	if (!colorTexturesID) {
		std::cerr << "Failed to load color textures." << std::endl;
	}

	metallicRoughnessTextureID = initTextureArrays(metallicRoughnessTexturesIndices);
	if (!metallicRoughnessTextureID)
	{
		std::cerr << "Failed to load metallic roughness textures." << std::endl;
	}
}


glm::mat4 GltfObject::getNodeTransform(const tinygltf::Node &node) {
	glm::mat4 transform(1.0f);

	if (node.matrix.size() == 16) {
		transform = glm::make_mat4(node.matrix.data());
	} else {
		if (node.translation.size() == 3) {
			transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
		}
		if (node.rotation.size() == 4) {
			const glm::quat q(static_cast<float>(node.rotation[3]), static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]));
			transform *= glm::mat4_cast(q);
		}
		if (node.scale.size() == 3) {
			transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
		}
	}
	return transform;
}

void GltfObject::computeLocalNodeTransform(const tinygltf::Model& model,
	const int nodeIndex,
	std::vector<glm::mat4> &localTransforms)
{
	localTransforms[nodeIndex] = getNodeTransform(model.nodes[nodeIndex]);

	const tinygltf::Node& node = model.nodes[nodeIndex];
	for (const int childIndex : node.children)
		computeLocalNodeTransform(model, childIndex, localTransforms);
}

void GltfObject::computeGlobalNodeTransform(const tinygltf::Model& model,
	const std::vector<glm::mat4> &localTransforms,
	int nodeIndex, const glm::mat4& parentTransform,
	std::vector<glm::mat4> &globalTransforms)
{
	const glm::mat4 globalTransform = parentTransform * localTransforms[nodeIndex];
	globalTransforms[nodeIndex] = globalTransform;

	const tinygltf::Node& node = model.nodes[nodeIndex];
	for(const int children_i: node.children)
		computeGlobalNodeTransform(model, localTransforms, children_i, globalTransform, globalTransforms);
}

std::vector<SkinObject> GltfObject::prepareSkinning(const tinygltf::Model &model){
	std::vector<SkinObject> skinObjects;

	// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.

	for (size_t i = 0; i < model.skins.size(); i++) {
		SkinObject skinObject;

		const tinygltf::Skin &skin = model.skins[i];

		// Read inverseBindMatrices
		const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
		assert(accessor.type == TINYGLTF_TYPE_MAT4);
		const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
		const auto *ptr = reinterpret_cast<const float *>(
			buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

		skinObject.inverseBindMatrices.resize(accessor.count);
		for (size_t j = 0; j < accessor.count; j++) {
			float m[16];
			memcpy(m, ptr + j * 16, 16 * sizeof(float));
			skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
		}

		assert(skin.joints.size() == accessor.count);

		skinObject.globalJointTransforms.resize(skin.joints.size());
		skinObject.jointMatrices.resize(skin.joints.size());

		// Compute local transforms at each node
		const int rootNodeIndex = skin.joints[0];
		std::vector<glm::mat4> localNodeTransforms(skin.joints.size());
		computeLocalNodeTransform(model, rootNodeIndex, localNodeTransforms);

		// Compute global transforms at each node
		glm::mat4 parentTransform(1.0f);
		computeGlobalNodeTransform(model, localNodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);

		for(const int j: skin.joints)
			skinObject.jointMatrices[j] = skinObject.globalJointTransforms[skin.joints[j]] * skinObject.inverseBindMatrices[j];

		// ----------------------------------------------

		skinObjects.push_back(skinObject);
	}
	return skinObjects;
}

int findKeyframeIndex(const std::vector<float>& times, float animationTime)
{
	int left = 0;
	int right = static_cast<int>(times.size()) - 1;

	while (left <= right) {
		int mid = (left + right) / 2;

		if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
			return mid;
		}
		else if (times[mid] > animationTime) {
			right = mid - 1;
		}
		else { // animationTime >= times[mid + 1]
			left = mid + 1;
		}
	}

	// Target not found
	return static_cast<int>(times.size()) - 2;
}

std::vector<AnimationObject> GltfObject::prepareAnimation(const tinygltf::Model &model)
{
	std::vector<AnimationObject> animationObjects;
	for (const auto &anim : model.animations) {
		AnimationObject animationObject;

		for (const auto &sampler : anim.samplers) {
			SamplerObject samplerObject;

			const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
			const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
			const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

			assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

			// Input (time) values
			samplerObject.input.resize(inputAccessor.count);

			const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];

			// Read input (time) values
			int stride = inputAccessor.ByteStride(inputBufferView);
			for (size_t i = 0; i < inputAccessor.count; ++i) {
				samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
			}

			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];

			// Output values
			samplerObject.output.resize(outputAccessor.count);

			for (size_t i = 0; i < outputAccessor.count; ++i) {

				if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
					memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
				} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
					memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
				} else {
					std::cout << "Unsupport accessor type ..." << std::endl;
				}

			}

			animationObject.samplers.push_back(samplerObject);
		}

		animationObjects.push_back(animationObject);
	}
	return animationObjects;
}

void GltfObject::updateAnimation(
	const tinygltf::Model &model,
	const tinygltf::Animation &anim,
	const AnimationObject &animationObject,
	float time,
	std::vector<glm::mat4> &nodeTransforms)
{
	// There are many channels so we have to accumulate the transforms
	for (const auto &channel : anim.channels) {

		int targetNodeIndex = channel.target_node;
		const auto &sampler = anim.samplers[channel.sampler];

		// Access output (value) data for the channel
		const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
		const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
		const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

		// Calculate current animation time (wrap if necessary)
		const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
		auto animationTime = static_cast<float>(fmod(time, times.back()));

		int keyframeIndex = findKeyframeIndex(times, animationTime);
		const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];

		if (channel.target_path == "translation") {
			glm::vec3 translation0, translation1;
			memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

			glm::vec3 translation = translation0;
			nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
		} else if (channel.target_path == "rotation") {
			glm::quat rotation0, rotation1;
			memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));

			glm::quat rotation = rotation0;
			nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
		} else if (channel.target_path == "scale") {
			glm::vec3 scale0, scale1;
			memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

			glm::vec3 scale = scale0;
			nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
		}
	}
}

void GltfObject::updateSkinning(const std::vector<glm::mat4> &nodeTransforms) {
	for(int i=0; i<model.skins.size(); i++) {
		const auto& skin = model.skins[i];
		auto& skinObject = skinObjects[i];
		const int rootNodeIndex = skin.joints[0];

		// Compute global transforms at each node
		glm::mat4 parentTransform(1.0f);

		computeGlobalNodeTransform(model, nodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);

		for(const int j: skin.joints)
			skinObject.jointMatrices[j] = skinObject.globalJointTransforms[skin.joints[j]] * skinObject.inverseBindMatrices[j];
	}
}

void GltfObject::update(float time) {
	if (!model.animations.empty()) {
		const tinygltf::Animation &animation = model.animations[0];
		const AnimationObject &animationObject = animationObjects[0];

		for(const auto & skin : model.skins) {
				std::vector<glm::mat4> nodeTransforms(skin.joints.size());
			for (auto & nodeTransform : nodeTransforms) {
				nodeTransform = glm::mat4(1.0);
			}

			updateAnimation(model, animation, animationObject, time, nodeTransforms);
			updateSkinning(nodeTransforms);
		}
	}
}

bool GltfObject::loadModel(tinygltf::Model &model, const char *filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    const bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res) {
        std::cout << "Failed to load glTF: " << filename << std::endl;
        return false;
    }
    std::cout << "Loaded glTF: " << filename << std::endl;
    return res;
}

void GltfObject::bindMesh(std::vector<PrimitiveObject>& primitiveObjects, const tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (const auto& primitive : mesh.primitives) {
        if (primitive.indices < 0) {
            std::cerr << "Invalid indices for primitive" << std::endl;
            continue;
        }

        PrimitiveObject primitiveObject;
        glGenVertexArrays(1, &primitiveObject.vao);
        glBindVertexArray(primitiveObject.vao);

        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

        GLuint indexBufferID;
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<long long>(indexBufferView.byteLength),
                     &indexBuffer.data[indexBufferView.byteOffset],
                     GL_STATIC_DRAW);

        primitiveObject.vbos[GL_ELEMENT_ARRAY_BUFFER] = indexBufferID;

        for (const auto& attrib : primitive.attributes) {
            int vaa = -1;
            if (attrib.first == "POSITION") vaa = 0;
            if (attrib.first == "NORMAL") vaa = 1;
            if (attrib.first == "TEXCOORD_0") vaa = 2;
        	if (attrib.first == "JOINTS_0") vaa = 3;
        	if (attrib.first == "WEIGHTS_0") vaa = 4;
        	if (attrib.first == "COLOR_0") vaa = 5;

            if (vaa < 0) continue;

            const tinygltf::Accessor& accessor = model.accessors[attrib.second];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            GLuint vertexBufferID;
            glGenBuffers(1, &vertexBufferID);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         static_cast<long long>(bufferView.byteLength),
                         &buffer.data[bufferView.byteOffset],
                         GL_STATIC_DRAW);

            int size = accessor.type == TINYGLTF_TYPE_SCALAR ? 1 : accessor.type;
            glVertexAttribPointer(vaa,
                                  size,
                                  accessor.componentType,
                                  accessor.normalized ? GL_TRUE : GL_FALSE,
                                  bufferView.byteStride ? static_cast<int>(bufferView.byteStride) : 0,
                                  BUFFER_OFFSET(0));

            glEnableVertexAttribArray(vaa);
            primitiveObject.vbos[vaa] = vertexBufferID;
        }

        primitiveObjects.push_back(primitiveObject);
        glBindVertexArray(0);
    }
}

void GltfObject::bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects,
					tinygltf::Model &model,
					tinygltf::Node &node) {
	// Bind buffers for the current mesh at the node
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
		glBindVertexArray(0);
	}

	// Recursive into children nodes
	for (int i : node.children) {
		assert((i >= 0) && (i < model.nodes.size()));
		bindModelNodes(primitiveObjects, model, model.nodes[i]);
	}
}

std::vector<PrimitiveObject> GltfObject::bindModel(tinygltf::Model &model) {
	std::vector<PrimitiveObject> primitiveObjects;

	const tinygltf::Scene &scene = model.scenes[model.defaultScene];
	for (int node : scene.nodes) {
		assert((node >= 0) && (node < model.nodes.size()));
		bindModelNodes(primitiveObjects, model, model.nodes[node]);
	}

	return primitiveObjects;
}


GLuint GltfObject::initTextureArrays(std::vector<int> textureIndices) const {
    GLuint textureArrayID;
    glGenTextures(1, &textureArrayID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);

    int width = 1024;
    int height = 1024;
    int layerCount = textureIndices.size();

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, layerCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::cout << "Loading textures..." << std::endl;
    for (int i = 0; i < layerCount; i++) {
        const auto img = model.images[model.textures[textureIndices[i]].source];

        if (img.width != width || img.height != height) {
            std::vector<unsigned char> resizedImage(width * height * 4);

            if (!stbir_resize_uint8(
                img.image.data(), img.width, img.height, 0,
                resizedImage.data(), width, height, 0, 4)) {
	            std::cerr << "Failed to resize texture" << std::endl;
            }

            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1,
                GL_RGBA, GL_UNSIGNED_BYTE, resizedImage.data());
        } else {
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1,
                GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
        }
    }

    return textureArrayID;
}

void GltfObject::loadMaterials() {
    for (int i=0; i<model.materials.size(); i++) {
    	const auto material = model.materials[i];
		Material materialData;

    	if (material.values.find("baseColorTexture") != material.values.end()) {
            int textureIndex = material.values.at("baseColorTexture").TextureIndex();

    		baseColorMaterialIndices[i] = colorTexturesCount++;
    		baseColorTexturesIndices.push_back(textureIndex);
    	}
    	else if (material.values.find("baseColorFactor") != material.values.end()) {
            const auto& colorFactor = material.values.at("baseColorFactor").ColorFactor();
            materialData.baseColorFactor = glm::vec4(colorFactor[0], colorFactor[1], colorFactor[2], colorFactor[3]);
    	}

        if (material.values.find("metallicRoughnessTexture") != material.values.end()) {
            int textureIndex = material.values.at("metallicRoughnessTexture").TextureIndex();

        	metallicRoughnessMaterialIndices[i] = metallicRoughnessTextureCount++;
        	metallicRoughnessTexturesIndices.push_back(textureIndex);
        }
    	if (material.additionalValues.find("emissiveFactor") != material.additionalValues.end()) {
            const auto& emissiveFactor = material.additionalValues.at("emissiveFactor").ColorFactor();
			materialData.emissiveFactor = glm::vec3(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);
        }

    	materialsData[i] = materialData;
    }
}

void GltfObject::drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
                           const tinygltf::Model &model, const tinygltf::Mesh &mesh, GLuint programID) {
    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        glBindVertexArray(primitiveObjects[i].vao);

        const tinygltf::Primitive& primitive = mesh.primitives[i];

        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];


    	GLint metMaterialIDLocation = glGetUniformLocation(programID, "metTextureIndex");
    	glUniform1i(metMaterialIDLocation,
			(metallicRoughnessMaterialIndices.find(primitive.material) != metallicRoughnessMaterialIndices.end()) ? metallicRoughnessTexturesIndices[primitive.material] : -1);

    	GLint materialIDLocation = glGetUniformLocation(programID, "colorTextureIndex");
    	glUniform1i(materialIDLocation, (baseColorMaterialIndices.find(primitive.material) != baseColorMaterialIndices.end()) ? baseColorMaterialIndices[primitive.material] : -1);

    	materialIDLocation = glGetUniformLocation(programID, "baseColorFactor");
    	glUniform4fv(materialIDLocation, 1, value_ptr(materialsData[primitive.material].baseColorFactor));

        glDrawElements(primitive.mode,
                       static_cast<int>(indexAccessor.count),
                       indexAccessor.componentType,
                       BUFFER_OFFSET(indexAccessor.byteOffset));

    	GLenum err;
    	while ((err = glGetError()) != GL_NO_ERROR) {
    		std::cerr << "OpenGL error: " << err << std::endl;
    	}

    	// Unbind to avoid contamination
    	glBindTexture(GL_TEXTURE_2D, 0);
    	glBindVertexArray(0);
    }
}

void GltfObject::drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
					tinygltf::Model &model, const tinygltf::Node &node, GLuint programID) {
	// Draw the mesh at the node, and recursively do so for children nodes
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		drawMesh(primitiveObjects, model, model.meshes[node.mesh], programID);
	}
	for (const int i : node.children) {
		drawModelNodes(primitiveObjects, model, model.nodes[i], programID);
	}
}

void GltfObject::drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
			tinygltf::Model &model, GLuint programID) {
	// Draw all nodes
	const tinygltf::Scene &scene = model.scenes[model.defaultScene];

	if (scene.nodes.empty()) {
		std::cerr << "Error: No nodes found in the default scene." << std::endl;
		return;
	}

	for (int node : scene.nodes) {
		drawModelNodes(primitiveObjects, model, model.nodes[node], programID);
	}
}

void GltfObject::render(const GLuint programID) {
	GraphicsObject::render(programID);

	const GLuint jointMatricesID = glGetUniformLocation(programID, "jointMatrices");

	for (const auto& skinObject : skinObjects)
		glUniformMatrix4fv(static_cast<GLint>(jointMatricesID), static_cast<int>(skinObject.jointMatrices.size()), GL_FALSE, glm::value_ptr(skinObject.jointMatrices[0]));

	glUniform1i(glGetUniformLocation(programID, "ignoreLightingPass"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, colorTexturesID);

	glUniform1i(glGetUniformLocation(programID, "textureArray"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metallicRoughnessTextureID);

	const GLint metMaterialIDLocation = glGetUniformLocation(programID, "animated");
	glUniform1i(metMaterialIDLocation, animated);

	// Draw the GLTF graphics_object
	drawModel(primitiveObjects, model, programID);
}
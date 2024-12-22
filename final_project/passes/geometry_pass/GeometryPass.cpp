//
// Created by miche on 22/12/2024.
//

#include "GeometryPass.h"

#include <iostream>
#include <render/shader.h>

GeometryPass::GeometryPass(const int width, const int height) : RenderPass(width, height, LoadShadersFromFile("../final_project/shaders/geometry.vert","../final_project/shaders/geometry.frag")) {
}

void GeometryPass::setup() {
    glBindFramebuffer(GL_FRAMEBUFFER, getFBO());

	// Position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getWidth(), getHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getWidth(), getHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// Color and specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	// World position color buffer
	glGenTextures(1, &gPositionWorld);
	glBindTexture(GL_TEXTURE_2D, gPositionWorld);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getWidth(), getHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gPositionWorld, 0);

	glGenTextures(1, &gIgnoreLightingPass);
	glBindTexture(GL_TEXTURE_2D, gIgnoreLightingPass);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, getWidth(), getHeight(), 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gIgnoreLightingPass, 0);

    const GLuint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, attachments);

	// Create and attach depth buffer
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getWidth(), getHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	//Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GeometryPass::render(const std::vector<GraphicsObject *> &objects, const Camera &camera) {
	glBindFramebuffer(GL_FRAMEBUFFER, getFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = camera.getProjectionMatrix();
	glm::mat4 view = camera.getViewMatrix();
	glUseProgram(getShaderID());
	glUniformMatrix4fv(glGetUniformLocation(getShaderID(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(getShaderID(), "view"), 1, GL_FALSE, &view[0][0]);

	for (const auto& object : objects) {
		glUniform1i(glGetUniformLocation(getShaderID(), "invertedNormals"), 0);
		object->render(getShaderID());
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GeometryPass::cleanup() {
	RenderPass::cleanup();

	if (gPosition != 0) {
		glDeleteTextures(1, &gPosition);
		gPosition = 0;
	}

	if (gPositionWorld != 0) {
		glDeleteTextures(1, &gPositionWorld);
		gPositionWorld = 0;
	}

	if (gNormal != 0) {
		glDeleteTextures(1, &gNormal);
		gNormal = 0;
	}

	if (gAlbedo != 0) {
		glDeleteTextures(1, &gAlbedo);
		gAlbedo = 0;
	}

	if (gIgnoreLightingPass != 0) {
		glDeleteTextures(1, &gIgnoreLightingPass);
		gIgnoreLightingPass = 0;
	}

	if (rboDepth != 0) {
		glDeleteRenderbuffers(1, &rboDepth);
		rboDepth = 0;
	}
}

GLuint GeometryPass::getGPosition() const {
	return gPosition;
}

GLuint GeometryPass::getGPositionWorld() const {
	return gPositionWorld;
}

GLuint GeometryPass::getGNormal() const {
	return gNormal;
}

GLuint GeometryPass::getGAlbedo() const {
	return gAlbedo;
}

GLuint GeometryPass::getGIgnoreLightingPass() const {
	return gIgnoreLightingPass;
}

GLuint GeometryPass::getRboDepth() const {
	return rboDepth;
}


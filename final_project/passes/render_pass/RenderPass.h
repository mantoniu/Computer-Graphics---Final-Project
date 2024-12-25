//
// Created by miche on 22/12/2024.
//

#ifndef RENDERCLASS_H
#define RENDERCLASS_H
#include <vector>
#include <view_points/camera/camera.h>
#include "3D_objects/graphics_object/GraphicsObject.h"


class RenderPass {
    private:
        GLuint FBO = 0;
        GLuint shaderID;
        int width, height;

    public:
        RenderPass(int width, int height, GLuint shader);
        virtual ~RenderPass() = default;

        virtual void setup() = 0;
        virtual void render(const std::vector<GraphicsObject*>& objects, const Camera& camera) = 0;
        virtual void cleanup();

        [[nodiscard]] GLuint getFBO() const;
        [[nodiscard]] int getWidth() const;
        [[nodiscard]] int getHeight() const;
        [[nodiscard]] GLuint getShaderID() const;
};


#endif //RENDERCLASS_H

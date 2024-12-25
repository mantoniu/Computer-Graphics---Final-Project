//
// Created by miche on 24/12/2024.
//

#ifndef VIEWPOINT_H
#define VIEWPOINT_H
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec3.hpp>

class ViewPoint {
    private:
        glm::vec3 position;
        glm::vec3 up = glm::vec3(0,1,0);
        float viewAzimuth;
        float viewPolar;

        float fov;
        float near;
        float far;

    public:
        virtual ~ViewPoint() = default;
        ViewPoint(glm::vec3 position, float viewAzimuth, float viewPolar, float fov, float near, float far);

        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] float getViewAzimuth() const;
        [[nodiscard]] float getViewPolar() const;
        [[nodiscard]] glm::vec3 getLookAt() const;
        [[nodiscard]] glm::vec3 getUp() const;

        void setPosition(glm::vec3 position);
        void setViewAzimuth(float viewAzimuth);
        void setViewPolar(float viewPolar);

        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] glm::mat4 getProjectionMatrix() const;
        [[nodiscard]] glm::mat4 getVPMatrix() const;
        [[nodiscard]] virtual float getAspectRatio() const = 0;
};



#endif //VIEWPOINT_H

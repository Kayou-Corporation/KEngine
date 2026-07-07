#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace Kayou::Core
{
    class Camera
    {
    public:
        virtual ~Camera() = default;

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);
        void SetFov(float fovRadian);
        void SetAspectRatio(float aspectRatio);
        void SetNearPlane(float nearPlane);
        void SetFarPlane(float farPlane);

        const glm::vec3& GetPosition() const;
        const glm::vec3& GetRotation() const;

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 GetViewProjectionMatrix() const;

        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;


        void RecalculateMatrices() const;
    protected:
        void MarkDirty() const;

        glm::vec3 m_position { 0.0f };
        glm::vec3 m_rotation { 0.0f }; // pitch, yaw, roll

        float m_fov = glm::radians(45.0f);
        float m_aspectRatio = 16.0f / 9.0f;
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;

        mutable bool m_dirty = true;
        mutable glm::mat4 m_viewMatrix { 1.0f };
        mutable glm::mat4 m_projectionMatrix { 1.0f };
        mutable glm::mat4 m_viewProjectionMatrix = { 1.0f };

    };
}

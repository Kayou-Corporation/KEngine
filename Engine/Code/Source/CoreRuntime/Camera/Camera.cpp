#include "Camera/Camera.hpp"

namespace Kayou::Core
{
    void Camera::SetPosition(const glm::vec3& position)
    {
        m_position = position;
        MarkDirty();
    }


    void Camera::SetRotation(const glm::vec3& rotationDegrees)
    {
        m_rotation = rotationDegrees;
        MarkDirty();
    }


    void Camera::SetFov(const float fovRadian)
    {
        m_fov = fovRadian;
        MarkDirty();
    }


    void Camera::SetAspectRatio(const float aspectRatio)
    {
        m_aspectRatio = aspectRatio;
        MarkDirty();
    }


    void Camera::SetNearPlane(const float nearPlane)
    {
        m_nearPlane = nearPlane;
        MarkDirty();
    }


    void Camera::SetFarPlane(const float farPlane)
    {
        m_farPlane = farPlane;
        MarkDirty();
    }


    const glm::vec3& Camera::GetPosition() const
    {
        return m_position;
    }


    const glm::vec3& Camera::GetRotation() const
    {
        return m_rotation;
    }


    glm::mat4 Camera::GetViewMatrix() const
    {
        RecalculateMatrices();
        return m_viewMatrix;
    }


    glm::mat4 Camera::GetProjectionMatrix() const
    {
        RecalculateMatrices();
        return m_projectionMatrix;
    }


    glm::mat4 Camera::GetViewProjectionMatrix() const
    {
        RecalculateMatrices();
        return m_viewProjectionMatrix;
    }


    glm::vec3 Camera::GetForward() const
    {
        const glm::vec3 rotationRad = glm::radians(m_rotation);
        const glm::vec3 forward
        {
            cos(rotationRad.y) * cos(rotationRad.x),
            sin(rotationRad.x),
            sin(rotationRad.y) * cos(rotationRad.x)
        };

        return glm::normalize(forward);
    }


    glm::vec3 Camera::GetRight() const
    {
        return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }


    glm::vec3 Camera::GetUp() const
    {
        return glm::normalize(glm::cross(GetRight(), GetForward()));
    }


    void Camera::MarkDirty() const
    {
        m_dirty = true;
    }


    void Camera::RecalculateMatrices() const
    {
        if (!m_dirty)
            return;

        const glm::vec3 forward = GetForward();
        const glm::vec3 up = GetUp();

        m_viewMatrix = glm::lookAt(m_position, forward, up);
        m_projectionMatrix = glm::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
        m_projectionMatrix[1][1] *= -1.0f; // Vulkan patch
        m_viewProjectionMatrix = glm::transpose(m_projectionMatrix * m_viewMatrix);

        m_dirty = false;
    }
}

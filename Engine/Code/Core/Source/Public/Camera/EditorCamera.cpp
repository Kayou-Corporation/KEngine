#include "Public/Camera/EditorCamera.hpp"

#include "Public/Camera/CameraInput.hpp"


namespace Kayou::Core
{
    void EditorCamera::Update(const float deltaTime, const CameraInput& input)
    {
        UpdateMovement(deltaTime, input);
        UpdateRotation(input.mouseDelta);
        ClampPitch();

        MarkDirty();
    }


    void EditorCamera::UpdateMovement(const float deltaTime, const CameraInput& input)
    {
        glm::vec3 position = m_position;
        const glm::vec3 forward = GetForward();
        const glm::vec3 right = GetRight();
        const glm::vec3 up = { 0.0f, 1.0f, 0.0f };

        const float velocity = m_moveSpeed * deltaTime;

        if (input.moveForward)
            position += forward * velocity;

        if (input.moveBackward)
            position -= forward * velocity;

        if (input.moveRight)
            position += right * velocity;

        if (input.moveLeft)
            position -= right * velocity;

        if (input.moveUp)
            position += up * velocity;

        if (input.moveDown)
            position -= up * velocity;

        m_position = position;
    }


    void EditorCamera::UpdateRotation(const glm::vec2 mouseDelta)
    {
        m_rotation.y += mouseDelta.x * m_mouseSensitivity;
        m_rotation.x -= mouseDelta.y * m_mouseSensitivity;
    }


    void EditorCamera::ClampPitch()
    {
        m_rotation.x = glm::clamp(m_rotation.x, -89.0f, 89.0f);
    }


    void EditorCamera::SetMoveSpeed(const float speed)
    {
        m_moveSpeed = speed;
    }


    void EditorCamera::SetMouseSensitivity(const float sensitivity)
    {
        m_mouseSensitivity = sensitivity;
    }
}

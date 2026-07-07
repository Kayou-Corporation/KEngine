#pragma once

#include "Camera/Camera.hpp"



namespace Kayou::Core
{
    struct CameraInput;


    class EditorCamera : public Camera
    {
    public:
        void Update(float deltaTime, const CameraInput& input);
        void SetMoveSpeed(float speed);
        void SetMouseSensitivity(float sensitivity);


    private:
        void UpdateMovement(float deltaTime, const CameraInput& input);
        void UpdateRotation(glm::vec2 mouseDelta);
        void ClampPitch();

        float m_moveSpeed = 5.0f;
        float m_mouseSensitivity = 0.1f;
    };
}

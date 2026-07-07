#pragma once

#include "glm/vec2.hpp"



namespace Kayou::Core
{
    struct CameraInput
    {
        bool moveForward = false;
        bool moveBackward = false;
        bool moveLeft = false;
        bool moveRight = false;
        bool moveUp = false;
        bool moveDown = false;

        glm::vec2 mouseDelta { 0.0f };
    };
}

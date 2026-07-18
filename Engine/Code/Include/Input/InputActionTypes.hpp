#pragma once

#include <cstdint>
#include <functional>

#include "glm/vec2.hpp"



namespace Kayou::Input
{
    enum class EActionValueType : std::uint8_t
    {
        Flag = 0, // bool (on/off)
        Axis1D = 1,  // float
        Axis2D = 2   // Vec2
    };


    // - Started   : First frame from which the value becomes "active" (ex: button pressed)
    // - Ongoing   : value is active and maintained (ex: axis non-zero, key held)
    // - Completed : the value becomes inactive again (ex: button released)
    enum class ETriggerEvent : std::uint8_t
    {
        Started = 0,
        Ongoing = 1,
        Completed = 2
    };


    // A single link "Physical Code -> Action Component".
    // `scale` allows us to convert digital inputs into axes (e.g., D=+1, Q=-1 on X).
    struct InputBinding
    {
        EDeviceType device = EDeviceType::Keyboard;
        std::uint16_t code = 0; // EKey / EMouseButton / EGamepadButton / EGamepadAxis based on `device`
        float scale = 1.0f;
        bool affectsY = false; // true => contributes to value.y instead of value.x (for Axis2D composite)
        float deadZone = 0.0f; // Useful for gamepads
    };


    struct ActionValue
    {
        EActionValueType type = EActionValueType::Flag;
        glm::vec2 current{};
        glm::vec2 previous{};

        bool AsBool() const
        {
            return current.x != 0.0f;
        }

        float AsFloat() const
        {
            return current.x;
        }

        glm::vec2 AsVec2() const
        {
            return current;
        }
    };


    using ActionCallback = std::function<void(const ActionValue&)>;
}

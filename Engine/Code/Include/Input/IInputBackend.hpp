#pragma once

#include <string_view>
#include <vector>

#include "InputTypes.hpp"



namespace Kayou::Input
{
    /// @brief Interface for input backends. Each backend (e.g., SDL3, GLFW, native OS) must implement this interface.
    ///        The rest of the engine (InputManager, InputAction, etc.) only interacts with this interface.
    class IInputBackend
    {
    public:
        virtual ~IInputBackend() = default;

        // Initializes the backend (returns false if it fails)
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;


        /// @brief Polls native input events and translates them into generic RawInputEvent, adding them to `outEvents`.
        ///        Must be called once per frame (before updating InputAction).
        virtual void PollEvents(std::vector<RawInputEvent>& outEvents) = 0;


        /// @brief Enables or disables text input mode.
        ///        This should only be enabled when the user is typing in a text field, and never during gameplay.
        virtual void SetTextInputMode(bool enabled) = 0;


        /// @brief Returns the localized display name of a key (for UI rebinding), e.g., "A", "Space".
        virtual std::string_view GetKeyDisplayName(EKey key) const = 0;


        /// @brief Enables or disables relative mouse mode (for FPS/TPS camera), decoupled from the system cursor.
        virtual void SetMouseRelativeMode(bool enabled) = 0;


        /// @brief Returns the name of the backend (for debugging/logging purposes).
        virtual std::string_view GetBackendName() const = 0;
    };
}
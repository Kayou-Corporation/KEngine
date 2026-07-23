#pragma once

#include <unordered_map>
#include <SDL3/SDL.h>

#include "../Input/IInputBackend.hpp"



namespace Kayou::Input
{
    /// @brief SDL3 implementation of the IInputBackend interface.
    class SDLInputBackend : public IInputBackend
    {
    public:
        explicit SDLInputBackend(SDL_Window* window) : m_window(window)
        {
        }

        bool Initialize() override;
        void Shutdown() override;
        void PumpEvents(std::vector<RawInputEvent>& outEvents) override;
        void SetTextInputMode(bool enabled) override;
        std::string_view GetKeyDisplayName(EKey key) const override;
        void SetMouseRelativeMode(bool enabled) override;

        std::string_view GetBackendName() const override
        {
            return "SDL3";
        }

        
    private:
        void HandleGamepadAdded(SDL_JoystickID id);
        void HandleGamepadRemoved(SDL_JoystickID id);

        static EKey ScancodeToEKey(SDL_Scancode code);
        static SDL_Scancode EKeyToScancode(EKey key);
        static EMouseButton SDLButtonToEMouseButton(uint8_t button);
        static EGamepadButton SDLGamepadButtonToEnum(SDL_GamepadButton button);
        static EGamepadAxis SDLGamepadAxisToEnum(SDL_GamepadAxis axis);

        SDL_Window* m_window = nullptr;
        std::unordered_map<SDL_JoystickID, SDL_Gamepad*> m_openGamepads;
        mutable std::string m_keyNameScratch; // buffer for GetKeyDisplayName
    };
}

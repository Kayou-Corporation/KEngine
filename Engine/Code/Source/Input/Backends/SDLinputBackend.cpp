#include <chrono>
#include <ranges>

#include "../Input/Backends/SDLInputBackend.hpp"


namespace Kayou::Input
{
    uint64_t NowNs()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }


    /// @brief Normalizes a raw SDL gamepad axis value (int16_t) to a float in the range [-1.0, 1.0].
    /// @param raw The raw axis value from SDL (typically in the range [-32768, 32767]).
    /// @return A float representing the normalized axis value.
    float NormalizeAxis16(const int16_t raw)
    {
        return raw < 0 ? static_cast<float>(raw) / 32768.0f : static_cast<float>(raw) / 32767.0f;
    }


    bool SDLInputBackend::Initialize()
    {
        // On suppose que SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | ...) a deja
        // ete appele en amont par le moteur (init generale de l'app, pas du ressort
        // de ce wrapper input). Ici on se contente d'ouvrir les manettes deja connectees.

        /// @note SDL3: SDL_GetGamepads() returns a dynamically allocated array of SDL_JoystickID, which must be freed with SDL_free().
        int count = 0;
        SDL_JoystickID* ids = SDL_GetGamepads(&count);
        if (ids)
        {
            for (int i = 0; i < count; ++i)
                HandleGamepadAdded(ids[i]);

            SDL_free(ids);
        }
        return true;
    }


    void SDLInputBackend::Shutdown()
    {
        for (SDL_Gamepad*& gamepad : m_openGamepads | std::views::values)
            SDL_CloseGamepad(gamepad);

        m_openGamepads.clear();
    }


    void SDLInputBackend::HandleGamepadAdded(const SDL_JoystickID id)
    {
        if (SDL_Gamepad* gamepad = SDL_OpenGamepad(id))
            m_openGamepads[id] = gamepad;
    }

    void SDLInputBackend::HandleGamepadRemoved(const SDL_JoystickID id)
    {
        const std::unordered_map<unsigned, SDL_Gamepad*>::iterator it = m_openGamepads.find(id);
        if (it != m_openGamepads.end())
        {
            SDL_CloseGamepad(it->second);
            m_openGamepads.erase(it);
        }
    }


    void SDLInputBackend::PumpEvents(std::vector<RawInputEvent>& outEvents)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            const uint64_t ts = NowNs();

            switch (event.type)
            {
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                {
                    // Scancode = position physique, recommande pour le gameplay
                    // (cf. "The 101-Button Joystick"). On ignore volontairement le
                    // key-repeat pour les actions de jeu : e.key.repeat != 0 => on drop,
                    // le systeme d'action gere l'etat "maintenu" via Ongoing, pas via
                    // des evenements repetes du systeme.
                    if (event.key.repeat) break;
                    EKey key = ScancodeToEKey(event.key.scancode);
                    if (key == EKey::Unknown) break;
                    outEvents.push_back(RawInputEvent{
                                            .device = EDeviceType::Keyboard,
                                            .code = static_cast<uint16_t>(key),
                                            .value = (event.type == SDL_EVENT_KEY_DOWN) ? 1.0f : 0.0f,
                                            .deviceId = 0,
                                            .timestampNs = ts
                                        });
                    break;
                }

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    EMouseButton btn = SDLButtonToEMouseButton(event.button.button);
                    outEvents.push_back(RawInputEvent{
                                            .device = EDeviceType::Mouse,
                                            .code = static_cast<uint16_t>(btn),
                                            .value = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? 1.0f : 0.0f,
                                            .deviceId = event.button.which,
                                            .timestampNs = ts
                                        });
                    break;
                }

                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                {
                    EGamepadButton btn = SDLGamepadButtonToEnum(static_cast<SDL_GamepadButton>(event.gbutton.button));
                    outEvents.push_back(RawInputEvent{
                                            .device = EDeviceType::Gamepad,
                                            .code = static_cast<uint16_t>(btn),
                                            .value = (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) ? 1.0f : 0.0f,
                                            .deviceId = static_cast<uint32_t>(event.gbutton.which),
                                            .timestampNs = ts
                                        });
                    break;
                }

                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                {
                    EGamepadAxis axis = SDLGamepadAxisToEnum(static_cast<SDL_GamepadAxis>(event.gaxis.axis));
                    outEvents.push_back(RawInputEvent{
                                            .device = EDeviceType::Gamepad,
                                            .code = static_cast<uint16_t>(axis),
                                            .value = NormalizeAxis16(event.gaxis.value),
                                            .deviceId = static_cast<uint32_t>(event.gaxis.which),
                                            .timestampNs = ts
                                        });
                    break;
                }

                case SDL_EVENT_GAMEPAD_ADDED:
                    HandleGamepadAdded(event.gdevice.which);
                    break;

                case SDL_EVENT_GAMEPAD_REMOVED:
                    HandleGamepadRemoved(event.gdevice.which);
                    break;

                default:
                    break;
            }
        }
    }

    void SDLInputBackend::SetTextInputMode(const bool enabled)
    {
        if (!m_window)
            return;

        if (enabled)
            SDL_StartTextInput(m_window);
        else
            SDL_StopTextInput(m_window);
    }

    void SDLInputBackend::SetMouseRelativeMode(bool enabled)
    {
        if (!m_window)
            return;
        // A VERIFIER : SDL_SetWindowRelativeMouseMode(SDL_Window*, bool) est le nom
        // dans les versions SDL3 recentes que je connais (renommage depuis
        // SDL_SetRelativeMouseMode de SDL2). Confirme sur SDL3/SDL_mouse.h.
        SDL_SetWindowRelativeMouseMode(m_window, enabled);
    }

    std::string_view SDLInputBackend::GetKeyDisplayName(const EKey key) const
    {
        const SDL_Scancode scancode = EKeyToScancode(key);
        const SDL_Keycode keycode = SDL_GetKeyFromScancode(scancode, SDL_KMOD_NONE, false);

        const char* name = SDL_GetKeyName(keycode);
        m_keyNameScratch = name ? name : "";

        return m_keyNameScratch;
    }

    // -----------------------------------------------------------------------
    // Tables de traduction. Sous-ensemble representatif : a completer pour
    // couvrir l'integralite de SDL_Scancode / SDL_GamepadButton / SDL_GamepadAxis
    // (voir SDL3/SDL_scancode.h et SDL3/SDL_gamepad.h pour la liste complete).
    // -----------------------------------------------------------------------

    EKey SDLInputBackend::ScancodeToEKey(const SDL_Scancode code)
    {
        switch (code)
        {
            case SDL_SCANCODE_A:
                return EKey::A;
            case SDL_SCANCODE_B:
                return EKey::B;
            case SDL_SCANCODE_C:
                return EKey::C;
            case SDL_SCANCODE_D:
                return EKey::D;
            case SDL_SCANCODE_E:
                return EKey::E;
            case SDL_SCANCODE_F:
                return EKey::F;
            case SDL_SCANCODE_G:
                return EKey::G;
            case SDL_SCANCODE_H:
                return EKey::H;
            case SDL_SCANCODE_I:
                return EKey::I;
            case SDL_SCANCODE_J:
                return EKey::J;
            case SDL_SCANCODE_K:
                return EKey::K;
            case SDL_SCANCODE_L:
                return EKey::L;
            case SDL_SCANCODE_M:
                return EKey::M;
            case SDL_SCANCODE_N:
                return EKey::N;
            case SDL_SCANCODE_O:
                return EKey::O;
            case SDL_SCANCODE_P:
                return EKey::P;
            case SDL_SCANCODE_Q:
                return EKey::Q;
            case SDL_SCANCODE_R:
                return EKey::R;
            case SDL_SCANCODE_S:
                return EKey::S;
            case SDL_SCANCODE_T:
                return EKey::T;
            case SDL_SCANCODE_U:
                return EKey::U;
            case SDL_SCANCODE_V:
                return EKey::V;
            case SDL_SCANCODE_W:
                return EKey::W;
            case SDL_SCANCODE_X:
                return EKey::X;
            case SDL_SCANCODE_Y:
                return EKey::Y;
            case SDL_SCANCODE_Z:
                return EKey::Z;
            case SDL_SCANCODE_SPACE:
                return EKey::Space;
            case SDL_SCANCODE_RETURN:
                return EKey::Return;
            case SDL_SCANCODE_ESCAPE:
                return EKey::Escape;
            case SDL_SCANCODE_LSHIFT:
                return EKey::LeftShift;
            case SDL_SCANCODE_RSHIFT:
                return EKey::RightShift;
            case SDL_SCANCODE_LCTRL:
                return EKey::LeftCtrl;
            case SDL_SCANCODE_RCTRL:
                return EKey::RightCtrl;
            case SDL_SCANCODE_UP:
                return EKey::ArrowUp;
            case SDL_SCANCODE_DOWN:
                return EKey::ArrowDown;
            case SDL_SCANCODE_LEFT:
                return EKey::ArrowLeft;
            case SDL_SCANCODE_RIGHT:
                return EKey::ArrowRight;
            // ... completer selon SDL_scancode.h
            default:
                return EKey::Unknown;
        }
    }

    SDL_Scancode SDLInputBackend::EKeyToScancode(const EKey key)
    {
        switch (key)
        {
            case EKey::A:
                return SDL_SCANCODE_A;
            case EKey::B:
                return SDL_SCANCODE_B;
            case EKey::W:
                return SDL_SCANCODE_W;
            case EKey::S:
                return SDL_SCANCODE_S;
            case EKey::D:
                return SDL_SCANCODE_D;
            case EKey::Space:
                return SDL_SCANCODE_SPACE;
            case EKey::Escape:
                return SDL_SCANCODE_ESCAPE;
            // ... completer (idealement generer cette table et la precedente
            // automatiquement a partir d'un seul fichier de definition pour eviter
            // toute divergence entre les deux sens de traduction)
            default:
                return SDL_SCANCODE_UNKNOWN;
        }
    }

    EMouseButton SDLInputBackend::SDLButtonToEMouseButton(const uint8_t button)
    {
        switch (button)
        {
            case SDL_BUTTON_LEFT:
                return EMouseButton::Left;
            case SDL_BUTTON_MIDDLE:
                return EMouseButton::Middle;
            case SDL_BUTTON_RIGHT:
                return EMouseButton::Right;
            case SDL_BUTTON_X1:
                return EMouseButton::X1;
            case SDL_BUTTON_X2:
                return EMouseButton::X2;
            default:
                return EMouseButton::Unknown;
        }
    }

    EGamepadButton SDLInputBackend::SDLGamepadButtonToEnum(const SDL_GamepadButton button)
    {
        switch (button)
        {
            case SDL_GAMEPAD_BUTTON_SOUTH:
                return EGamepadButton::South;
            case SDL_GAMEPAD_BUTTON_EAST:
                return EGamepadButton::East;
            case SDL_GAMEPAD_BUTTON_WEST:
                return EGamepadButton::West;
            case SDL_GAMEPAD_BUTTON_NORTH:
                return EGamepadButton::North;
            case SDL_GAMEPAD_BUTTON_BACK:
                return EGamepadButton::Back;
            case SDL_GAMEPAD_BUTTON_GUIDE:
                return EGamepadButton::Guide;
            case SDL_GAMEPAD_BUTTON_START:
                return EGamepadButton::Start;
            case SDL_GAMEPAD_BUTTON_LEFT_STICK:
                return EGamepadButton::LeftStick;
            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                return EGamepadButton::RightStick;
            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                return EGamepadButton::LeftShoulder;
            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                return EGamepadButton::RightShoulder;
            case SDL_GAMEPAD_BUTTON_DPAD_UP:
                return EGamepadButton::DPadUp;
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                return EGamepadButton::DPadDown;
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                return EGamepadButton::DPadLeft;
            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                return EGamepadButton::DPadRight;
            default:
                return EGamepadButton::Unknown;
        }
    }

    EGamepadAxis SDLInputBackend::SDLGamepadAxisToEnum(const SDL_GamepadAxis axis)
    {
        switch (axis)
        {
            case SDL_GAMEPAD_AXIS_LEFTX:
                return EGamepadAxis::LeftX;
            case SDL_GAMEPAD_AXIS_LEFTY:
                return EGamepadAxis::LeftY;
            case SDL_GAMEPAD_AXIS_RIGHTX:
                return EGamepadAxis::RightX;
            case SDL_GAMEPAD_AXIS_RIGHTY:
                return EGamepadAxis::RightY;
            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                return EGamepadAxis::LeftTrigger;
            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                return EGamepadAxis::RightTrigger;
            default:
                return EGamepadAxis::Unknown;
        }
    }
} // namespace engine::input

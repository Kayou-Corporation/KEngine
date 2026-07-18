#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../Input/InputActionTypes.hpp"



namespace Kayou::Input
{
    class InputAction
    {
    public:
        InputAction(std::string name, EActionValueType type) : m_name(std::move(name)), m_type(type) { }

        InputAction& AddBinding(InputBinding binding);
        void BindCallback(ETriggerEvent event, ActionCallback callback);


        const std::string& GetName() const;
        EActionValueType GetType() const;
        const std::vector<InputBinding>& GetBindings() const;
        const ActionValue& GetValue() const;


        // Called by InputManager
        void Internal_ApplyRawEvent(const RawInputEvent& evt);
        void Internal_EndFrame(); // Transitions from current to previous and triggers callbacks
        bool Internal_HasCallbacksFor(ETriggerEvent event) const;
        const std::vector<ActionCallback>& Internal_CallbacksFor(ETriggerEvent event) const;


    private:
        std::string m_name;
        EActionValueType m_type;
        std::vector<InputBinding> m_bindings;
        std::vector<float> m_bindingValues;
        // Last value (post-scale/deadzone) for each binding
        ActionValue m_value;
        std::unordered_map<ETriggerEvent, std::vector<ActionCallback>> m_callbacks;
    };
}
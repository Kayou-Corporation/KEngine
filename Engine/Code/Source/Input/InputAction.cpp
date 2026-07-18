#include "../Input/InputAction.hpp"


namespace Kayou::Input
{
    InputAction& InputAction::AddBinding(const InputBinding binding)
    {
        m_bindings.push_back(binding);
        m_bindingValues.push_back(0.0f);

        return *this;
    }


    void InputAction::BindCallback(const ETriggerEvent event, ActionCallback callback)
    {
        m_callbacks[event].push_back(std::move(callback));
    }


    const std::string& InputAction::GetName() const
    {
        return m_name;
    }


    EActionValueType InputAction::GetType() const
    {
        return m_type;
    }


    const std::vector<InputBinding>& InputAction::GetBindings() const
    {
        return m_bindings;
    }


    const ActionValue& InputAction::GetValue() const
    {
        return m_value;
    }


    void InputAction::Internal_EndFrame()
    {
    }


    bool InputAction::Internal_HasCallbacksFor(const ETriggerEvent event) const
    {
        const std::unordered_map<ETriggerEvent, std::vector<std::function<void(const ActionValue&)> > >::const_iterator it = m_callbacks.find(event);

        return it != m_callbacks.end() && !it->second.empty();
    }


    const std::vector<ActionCallback>& InputAction::Internal_CallbacksFor(const ETriggerEvent event) const
    {
        static const std::vector<ActionCallback> empty;
        const std::unordered_map<ETriggerEvent, std::vector<std::function<void(const ActionValue&)> > >::const_iterator it = m_callbacks.find(event);

        return it != m_callbacks.end() ? it->second : empty;
    }
}
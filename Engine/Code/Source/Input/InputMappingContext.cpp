#include "../Input/InputMappingContext.hpp"

#include <memory>
#include <string>
#include <unordered_map>


namespace Kayou::Input
{
    InputAction& InputMappingContext::AddAction(const std::string& name, EActionValueType type)
    {
        std::unique_ptr<InputAction> action = std::make_unique<InputAction>(name, type);
        InputAction& ref = *action;

        m_actions[name] = std::move(action);

        return ref;
    }


    InputAction* InputMappingContext::FindAction(const std::string& name)
    {
        const std::unordered_map<std::string, std::unique_ptr<InputAction> >::iterator it = m_actions.find(name);

        return it != m_actions.end() ? it->second.get() : nullptr;
    }


    const std::string& InputMappingContext::GetName() const
    {
        return m_name;
    }


    int InputMappingContext::GetPriority() const
    {
        return m_priority;
    }


    bool InputMappingContext::IsEnabled() const
    {
        return m_enabled;
    }


    void InputMappingContext::SetEnabled(bool enabled)
    {
        m_enabled = enabled;
    }


    std::unordered_map<std::string, std::unique_ptr<InputAction> >& InputMappingContext::Internal_Actions()
    {
        return m_actions;
    }
}

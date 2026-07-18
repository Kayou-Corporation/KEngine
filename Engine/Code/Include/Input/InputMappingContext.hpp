#pragma once

#include <algorithm>

#include "InputAction.hpp"



namespace Kayou::Input
{
    class InputMappingContext
    {
    public:
        explicit InputMappingContext(std::string name, const int priority = 0) : m_name(std::move(name)), m_priority(priority) { }

        InputAction& AddAction(const std::string& name, EActionValueType type);
        InputAction* FindAction(const std::string& name);

        const std::string& GetName() const;
        int GetPriority() const;
        bool IsEnabled() const;
        void SetEnabled(bool enabled);
        std::unordered_map<std::string, std::unique_ptr<InputAction> >& Internal_Actions();


    private:
        std::string m_name;
        int m_priority = -1;
        bool m_enabled = true;
        std::unordered_map<std::string, std::unique_ptr<InputAction>> m_actions = std::unordered_map<std::string, std::unique_ptr<InputAction>>();
    };
}

#include <ranges>

#include "../../Include/Input/InputManager.hpp"
#include "../../Include/Input/InputTypes.hpp"



namespace Kayou::Input
{
    InputMappingContext& InputManager::AddMappingContext(std::string name, int priority)
    {
        std::unique_ptr<InputMappingContext> ctx = std::make_unique<InputMappingContext>(std::move(name), priority);
        InputMappingContext& ref = *ctx;
        m_contexts.push_back(std::move(ctx));

        std::ranges::sort(m_contexts,
                          [](const std::unique_ptr<InputMappingContext>& a, const std::unique_ptr<InputMappingContext>& b)
                          {
                              return a->GetPriority() > b->GetPriority();
                          });
        return ref;
    }


    void InputManager::RemoveMappingContext(const std::string& name)
    {
        std::erase_if(m_contexts, [&](const std::unique_ptr<InputMappingContext>& ctx)
        {
            return ctx->GetName() == name;
        });
    }


    void InputManager::Update()
    {
        if (!m_backend) return;

        m_rawEvents.clear();
        m_backend->PumpEvents(m_rawEvents);

        for (const RawInputEvent& event : m_rawEvents)
        {
            for (const std::unique_ptr<InputMappingContext>& ctxPtr : m_contexts)
            {
                if (!ctxPtr->IsEnabled())
                    continue;

                for (const std::unique_ptr<InputAction>& action : ctxPtr->Internal_Actions() | std::views::values)
                    action->Internal_ApplyRawEvent(event);
            }
        }

        for (const std::unique_ptr<InputMappingContext>& ctxPtr : m_contexts)
            for (const std::unique_ptr<InputAction>& action : ctxPtr->Internal_Actions() | std::views::values)
                action->Internal_EndFrame();
    }


    InputAction* InputManager::FindAction(const std::string& name) const
    {
        for (const std::unique_ptr<InputMappingContext>& ctxPtr : m_contexts)
            if (InputAction* action = ctxPtr->FindAction(name))
                return action;

        return nullptr;
    }


    IInputBackend* InputManager::GetBackend()
    {
        return m_backend.get();
    }
};

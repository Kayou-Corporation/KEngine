#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "IInputBackend.hpp"
#include "InputMappingContext.hpp"



namespace Kayou::Input
{
    class IInputBackend;

    class InputManager
    {
    public:
        /// @brief Injects the input backend for the InputManager, as it doesn't automatically create the backend.
        ///        This allows us to link SDLInputBackend without touching the rest of the engine (dependency inversion).
        /// @param backend A unique pointer to an object implementing the IInputBackend interface.
        void SetBackend(std::unique_ptr<IInputBackend> backend)
        {
            m_backend = std::move(backend);
        }


        bool Initialize() const
        {
            return m_backend && m_backend->Initialize();
        }


        void Shutdown() const
        {
            if (m_backend)
                m_backend->Shutdown();
        }


        /// @brief Implements a mapping context, sorted by priority (highest first).
        ///         The most prioritized context receives/consumes events first.
        ///         Useful for input consumption.
        /// @param name The name of the mapping context
        /// @param priority The priority of the mapping context
        /// @return A reference to the created mapping context
        InputMappingContext& AddMappingContext(std::string name, int priority = 0);


        void RemoveMappingContext(const std::string& name);


        // Call once per frame, before any gameplay logic that reads the actions
        void Update();


        // Looks for an action using its name, in the current active contexts, sorted by priority order
        InputAction* FindAction(const std::string& name) const;


        IInputBackend* GetBackend();


    private:
        std::unique_ptr<IInputBackend> m_backend;
        std::vector<std::unique_ptr<InputMappingContext> > m_contexts;
        std::vector<struct RawInputEvent> m_rawEvents;
    };
}

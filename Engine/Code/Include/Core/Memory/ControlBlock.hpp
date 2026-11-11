#pragma once

#include "CoreModule.hpp"

#include <atomic>
BEGIN_NAMESPACE_CORE

struct ControlBlock
{
    // Number of references
    std::atomic<uint32_t> m_refCount{ 1 };

    // Add a reference to the smart pointer
    void AddReference() noexcept
    {
        m_refCount.fetch_add(1, std::memory_order_relaxed);
    }

    // Release a reference from the smart pointer
    bool ReleaseReference() noexcept
    {
        // fetch_sub return the value before it's actually decreased, this check is the number of reference is 0
        if (m_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            // If you only have 0 reference the object has to be destroy
            return true;
        }
        return false;
    }
};
END_NAMESPACE_CORE
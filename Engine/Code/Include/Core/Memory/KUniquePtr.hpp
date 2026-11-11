#pragma once

#include "CoreModule.hpp"
#include "spdlog/fmt/bundled/chrono.h"

BEGIN_NAMESPACE_CORE

template<typename T>
class KUniquePtr
{
    template<typename U> friend class KUniquePtr;

public:
    // Default constructor : just create with a nullptr pointer, it's user responsability
    // Remark : Do I really want to allow user to create a shared_ptr from nothing ?
    constexpr KUniquePtr() noexcept
    : m_ptr(nullptr) {}

    // Null constructor : just create with a nullptr pointer, it's user responsability
    // Remark : Do I really want to allow user to create a shared_ptr from nothing ?
    constexpr KUniquePtr(std::nullptr_t) noexcept
    : m_ptr(nullptr) {}

    // Constructor from a raw pointer
    explicit KUniquePtr(T* ptr) noexcept
    : m_ptr(ptr) {}

    // Move constructor
    KUniquePtr(KUniquePtr&& other) noexcept
    : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    // Conversion move constructor
    template<typename U>
    requires std::convertible_to<U*, T*>
    KUniquePtr(KUniquePtr<U>&& other) noexcept
    : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    // Move Assignment cosntructor
    KUniquePtr& operator=(KUniquePtr&& other) noexcept
    {
        if (this != &other)
        {
            Reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U*, T*>
    KUniquePtr& operator=(KUniquePtr<U>&& other) noexcept
    {
        if (static_cast<const void*>(this) != static_cast<const void*>(&other))
        {
            Reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    KUniquePtr& operator=(std::nullptr_t) noexcept
    {
        Reset();
        return *this;
    }

    // No copy constructor for unique ptr
    KUniquePtr(const KUniquePtr&) = delete;
    KUniquePtr& operator=(const KUniquePtr&) = delete;
    template<typename U>
    KUniquePtr(const KUniquePtr<U>&) = delete;
    template<typename U>
    KUniquePtr& operator=(const KUniquePtr<U>&) = delete;

    // Default destructor (only) : just delete pointer
    ~KUniquePtr()
    {
        Reset();
    }

    T* operator->() const noexcept { return m_ptr; }
    T& operator*() const noexcept { return *m_ptr; }
    explicit operator bool() const noexcept { return m_ptr != nullptr; }

    T* Get() const noexcept { return m_ptr; }

    T* Release() noexcept
    {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    void Reset(T* ptr = nullptr) noexcept
    {
        T* oldPtr = m_ptr;
        m_ptr = ptr;
        if (oldPtr)
        {
            delete oldPtr;
        }
    }

private:
    T* m_ptr;
};

template<typename T, typename... Args>
KUniquePtr<T> MakeUnique(Args&&... args)
{
    return KUniquePtr<T>(new T(std::forward<Args>(args)...));
}


END_NAMESPACE_CORE
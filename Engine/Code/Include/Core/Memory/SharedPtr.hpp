#pragma once

#include "CoreModule.hpp"

#include "ControlBlock.hpp"

#include <atomic>
BEGIN_NAMESPACE_CORE

template<typename T>
class SharedPtr
{
    template<typename U> friend class SharedPtr;

public:
    // Default constructor : just create with a nullptr pointer, it's user responsability
    // Remark : Do I really want to allow user to create a shared_ptr from nothing ?
    constexpr SharedPtr() noexcept
    : m_ptr(nullptr), m_controlBlock(nullptr) {}

    // Null constructor : just create with a nullptr pointer, it's user responsability
    // Remark : Do I really want to allow user to create a shared_ptr from nothing ?
    constexpr SharedPtr(std::nullptr_t) noexcept
    : m_ptr(nullptr), m_controlBlock(nullptr) {}

    // Copy constructor
    SharedPtr(const SharedPtr& other) noexcept
    : m_ptr(other.m_ptr), m_controlBlock(other.m_controlBlock)
    {
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
    }

    // Constructor from a raw pointer
    // Remark : Do I really want to allow user to create a shared_ptr from a raw pointer ?
    explicit SharedPtr(T* ptr) noexcept
    : m_ptr(ptr)
    {
        m_controlBlock = new ControlBlock();
    }

    // Move constructor
    SharedPtr(SharedPtr&& other) noexcept
    : m_ptr(other.m_ptr), m_controlBlock(other.m_controlBlock)
    {
        other.m_ptr = nullptr;
        other.m_controlBlock = nullptr;
    }

    // Conversion constructor
    template<typename U>
    requires std::convertible_to<U*, T*>
    SharedPtr(const SharedPtr<U>& other) noexcept
    : m_ptr(other.m_ptr),m_controlBlock(other.m_controlBlock)
    {
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
    }

    // Move conversion constructor
    template<typename U>
    requires std::convertible_to<U*, T*>
    SharedPtr(SharedPtr<U>&& other) noexcept
    : m_ptr(other.m_ptr), m_controlBlock(other.m_controlBlock)
    {
        other.m_ptr = nullptr;
        other.m_controlBlock = nullptr;
    }

    // Aliasing constructor
    template<typename Y>
    SharedPtr(const SharedPtr<Y> &other, T *ptr) noexcept
    : m_ptr(ptr), m_controlBlock(other.m_controlBlock)
    {
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept
    {
        if (this != &other)
        {
            Reset();

            m_ptr = other.m_ptr;
            m_controlBlock = other.m_controlBlock;

            other.m_ptr = nullptr;
            other.m_controlBlock = nullptr;
        }
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U*, T*>
    SharedPtr& operator=(SharedPtr<U>&& other) noexcept
    {
        Reset();

        m_ptr = other.m_ptr;
        m_controlBlock = other.m_controlBlock;

        other.m_ptr = nullptr;
        other.m_controlBlock = nullptr;

        return *this;
    }

    // Default destructor (only) : just decrease ref count and if == 0, delete pointer
    ~SharedPtr() noexcept
    {
       Reset();
    }

    // Operators :
    T* operator->() const { return m_ptr; }

    T& operator*() const { return *m_ptr; }

    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    SharedPtr& operator=(std::nullptr_t) noexcept
    {
        Reset();
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept
    {
        if (this != &other)
        {
            Reset();
            m_ptr = other.m_ptr;
            m_controlBlock = other.m_controlBlock;
            if (m_controlBlock)
            {
                m_controlBlock->AddReference();
            }
        }
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U*, T*>
    SharedPtr& operator=(const SharedPtr<U>& other) noexcept
    {
        Reset();
        m_ptr = other.m_ptr;
        m_controlBlock = other.m_controlBlock;
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
        return *this;
    }

    template<typename U>
    bool operator==(const SharedPtr<U>& other) const noexcept
    {
        return m_ptr == other.Get();
    }

    template<typename U>
    bool operator!=(const SharedPtr<U>& other) const noexcept
    {
        return m_ptr != other.Get();
    }

    bool operator==(std::nullptr_t) const noexcept
    {
        return m_ptr == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
        return m_ptr != nullptr;
    }

    uint32_t GetRefCount() const
    {
        if (m_controlBlock)
        {
            return m_controlBlock->m_refCount.load();
        }

        return 0;
    }

    T* Get() {return m_ptr;}

    void Reset()
    {
        if (m_ptr && m_controlBlock)
        {
            bool releasePointer = m_controlBlock->ReleaseReference();
            if (releasePointer)
            {
                delete m_ptr;
                delete m_controlBlock;
            }
        }
        m_ptr = nullptr;
        m_controlBlock = nullptr;
    }

private:
    T* m_ptr;
    ControlBlock* m_controlBlock;
};

END_NAMESPACE_CORE
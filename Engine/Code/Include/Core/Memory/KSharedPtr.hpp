#pragma once

#include "CoreModule.hpp"

#include "ControlBlock.hpp"

#include <atomic>
BEGIN_NAMESPACE_CORE

template<typename T>
class KSharedPtr
{
    template<typename U> friend class KSharedPtr;

    template<typename Target, typename Source>
    friend KSharedPtr<Target> StaticCast(const KSharedPtr<Source>& other) noexcept;

    template<typename Target, typename Source>
    friend KSharedPtr<Target> DynamicCast(const KSharedPtr<Source>& other) noexcept;


public:
    // Default constructor : just create with a nullptr pointer, it's user responsability
    // Remark : Do I really want to allow user to create a shared_ptr from nothing ?
    constexpr KSharedPtr() noexcept
    : m_ptr(nullptr), m_controlBlock(nullptr) {}

    // Null constructor : just create with a nullptr pointer, it's user responsability
    // Remark : Do I really want to allow user to create a shared_ptr from nothing ?
    constexpr KSharedPtr(std::nullptr_t) noexcept
    : m_ptr(nullptr), m_controlBlock(nullptr) {}

    // Copy constructor
    KSharedPtr(const KSharedPtr& other) noexcept
    : m_ptr(other.m_ptr), m_controlBlock(other.m_controlBlock)
    {
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
    }

    // Constructor from a raw pointer
    explicit KSharedPtr(T* ptr) noexcept
    : m_ptr(ptr)
    {
        if (m_ptr)
        {
            m_controlBlock = new ControlBlock();
        }
    }

    // Move constructor
    KSharedPtr(KSharedPtr&& other) noexcept
    : m_ptr(other.m_ptr), m_controlBlock(other.m_controlBlock)
    {
        other.m_ptr = nullptr;
        other.m_controlBlock = nullptr;
    }

    // Conversion copy constructor
    template<typename U>
    requires std::convertible_to<U*, T*>
    KSharedPtr(const KSharedPtr<U>& other) noexcept
    : m_ptr(other.m_ptr),m_controlBlock(other.m_controlBlock)
    {
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
    }

    // Conversion move constructor
    template<typename U>
    requires std::convertible_to<U*, T*>
    KSharedPtr(KSharedPtr<U>&& other) noexcept
    : m_ptr(other.m_ptr), m_controlBlock(other.m_controlBlock)
    {
        other.m_ptr = nullptr;
        other.m_controlBlock = nullptr;
    }

    KSharedPtr& operator=(KSharedPtr&& other) noexcept
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
    KSharedPtr& operator=(KSharedPtr<U>&& other) noexcept
    {
        Reset();

        m_ptr = other.m_ptr;
        m_controlBlock = other.m_controlBlock;

        other.m_ptr = nullptr;
        other.m_controlBlock = nullptr;

        return *this;
    }

    // Default destructor (only) : just decrease ref count and if == 0, delete pointer
    ~KSharedPtr() noexcept
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

    KSharedPtr& operator=(std::nullptr_t) noexcept
    {
        Reset();
        return *this;
    }

    KSharedPtr& operator=(const KSharedPtr& other) noexcept
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
    KSharedPtr& operator=(const KSharedPtr<U>& other) noexcept
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
    bool operator==(const KSharedPtr<U>& other) const noexcept
    {
        return m_ptr == other.Get();
    }

    template<typename U>
    bool operator!=(const KSharedPtr<U>& other) const noexcept
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

    T* Get() const noexcept {return m_ptr;}

    void Reset()
    {
        if (m_controlBlock)
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
    KSharedPtr(T* ptr, ControlBlock* block) noexcept
        : m_ptr(ptr)
        , m_controlBlock(block)
    {
        if (m_controlBlock)
        {
            m_controlBlock->AddReference();
        }
    }

    T* m_ptr;
    ControlBlock* m_controlBlock;
};

template<typename T, typename... Args>
KSharedPtr<T> MakeShared(Args&&... args)
{
    return KSharedPtr<T>(new T(std::forward<Args>(args)...));
}

template<typename T, typename U>
KSharedPtr<T> StaticCast(const KSharedPtr<U>& other) noexcept
{
    if (!other) return KSharedPtr<T>();

    T* castedPtr = static_cast<T*>(other.Get());
    return KSharedPtr<T>(castedPtr, other.m_controlBlock);
}

template<typename T, typename U>
KSharedPtr<T> DynamicCast(const KSharedPtr<U>& other) noexcept
{
    if (!other) return KSharedPtr<T>();

    if (T* castedPtr = dynamic_cast<T*>(other.Get()))
    {
        return KSharedPtr<T>(castedPtr, other.m_controlBlock);
    }

    return KSharedPtr<T>();
}

END_NAMESPACE_CORE
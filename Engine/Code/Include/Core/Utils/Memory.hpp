#pragma once

#include <atomic>
#include <spdlog/spdlog.h>
#include "CoreModule.hpp"
#include <concepts>

BEGIN_NAMESPACE_CORE

// Intrusive Ptr for owned resources
// RULE : All class derived from this MUST have a virtual destructor
class IResource
{
public:
    virtual ~IResource() = default;

    // Default value to enable KUniquePtr to work, get override properly for KSharedPtr in RefCounter class
    virtual unsigned long AddRef() { return 1; };
    virtual unsigned long Release() { delete this; return 0; };
    virtual unsigned long GetRefCount() { return 1; };
};

template<typename T>
class RefCounter : public T
{
private:
    std::atomic<unsigned long> m_count{ 1 };

public:
    RefCounter() = default;

    template<typename... Args>
    RefCounter(Args&&... args) : T(std::forward<Args>(args)...) {}
    virtual ~RefCounter() = default;

    unsigned long AddRef() override
    {
        return ++m_count;
    }

    unsigned long Release() override
    {
        unsigned long currentCount = --m_count;
        if (currentCount == 0)
        {
            delete this;
        }
        return currentCount;
    }

    unsigned long GetRefCount() override { return m_count.load(); }
};

template<typename T>
class KSharedPtr
{
    template<typename U> friend class KSharedPtr;

private:
    T* m_ptr = nullptr;

public:
    KSharedPtr() = default;

    explicit KSharedPtr(T* p) : m_ptr(p) { if (m_ptr) m_ptr->AddRef(); }

    KSharedPtr(std::nullptr_t) noexcept : m_ptr(nullptr) {}

    KSharedPtr(const KSharedPtr& other) : m_ptr(other.m_ptr) { if (m_ptr) m_ptr->AddRef(); }

    template<typename U>
    KSharedPtr(const KSharedPtr<U>& other) : m_ptr(other.m_ptr) { if (m_ptr) m_ptr->AddRef(); }

    KSharedPtr& operator=(const KSharedPtr& other)
    {
        if (this != &other)
        {
            T* oldPtr = m_ptr;
            m_ptr = other.m_ptr;

            if (m_ptr) m_ptr->AddRef();
            if (oldPtr) oldPtr->Release();
        }
        return *this;
    }

    template<typename U>
    KSharedPtr& operator=(const KSharedPtr<U>& other)
    {
        if (this->m_ptr != other.m_ptr)
        {
            T* oldPtr = m_ptr;
            m_ptr = other.m_ptr;

            if (m_ptr) m_ptr->AddRef();
            if (oldPtr) oldPtr->Release();
        }
        return *this;
    }

    KSharedPtr(KSharedPtr&& other) noexcept : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    template<typename U>
    KSharedPtr(KSharedPtr<U>&& other) noexcept : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    KSharedPtr& operator=(KSharedPtr&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    template<typename U>
    KSharedPtr& operator=(KSharedPtr<U>&& other) noexcept
    {
        if (this->m_ptr != other.m_ptr)
        {
            Release();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    ~KSharedPtr() { Release(); }

    void Release()
    {
        if (m_ptr)
        {
            T* temp = m_ptr;
            m_ptr = nullptr;
            temp->Release();
        }
    }

    void Attach(T* p)
    {
        Release();
        m_ptr = p;
    }

    T* Detach() noexcept
    {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    template<typename CastType>
    KSharedPtr<CastType> CastAs() const
    {
        if (!m_ptr)
        {
            return nullptr;
        };

        CastType* casted = dynamic_cast<CastType*>(m_ptr);
        if (!casted)
        {
            return nullptr;
        };

        KSharedPtr<CastType> result;
        result.m_ptr = casted;
        result.m_ptr->AddRef();
        return result;
    }

    T* operator->() const { return m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* Get() const { return m_ptr; }
    explicit operator bool() const { return m_ptr != nullptr; }
};

template<typename T, typename... Args>
KSharedPtr<T> CreateRefPtr(Args&&... args)
{
    KSharedPtr<T> ptr;
    ptr.Attach(new RefCounter<T>(std::forward<Args>(args)...));
    return ptr;
}

template<typename T>
class KUniquePtr
{
    template<typename U> friend class KUniquePtr;

private:
    T* m_ptr = nullptr;

public:
    // Basic constructors
    KUniquePtr() = default;
    explicit KUniquePtr(T* p) noexcept : m_ptr(p) {}
    KUniquePtr(std::nullptr_t) noexcept : m_ptr(nullptr) {}

    // No copy constructor for unique ptr
    KUniquePtr(const KUniquePtr&) = delete;
    KUniquePtr& operator=(const KUniquePtr&) = delete;
    template<typename U>
    KUniquePtr(const KUniquePtr<U>&) = delete;
    template<typename U>
    KUniquePtr& operator=(const KUniquePtr<U>&) = delete;

    // Only move constructor
    KUniquePtr(KUniquePtr&& other) noexcept : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    template<typename U>
    KUniquePtr(KUniquePtr<U>&& other) noexcept : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

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
    KUniquePtr& operator=(KUniquePtr<U>&& other) noexcept
    {
        if (this->m_ptr != other.m_ptr)
        {
            Reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    ~KUniquePtr() { Reset(); }

    void Reset(T* p = nullptr) noexcept
    {
        if (m_ptr)
        {
            T* temp = m_ptr;
            m_ptr = nullptr;
            temp->Release();
        }
        m_ptr = p;
    }

    void Attach(T* p) noexcept
    {
        Reset(p);
    }

    T* Detach() noexcept
    {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    template<typename CastType>
    KUniquePtr<CastType> CastAs() noexcept
    {
        if (!m_ptr) return {};
        CastType* casted = dynamic_cast<CastType*>(m_ptr);
        if (casted)
        {
            m_ptr = nullptr;
            return KUniquePtr<CastType>(casted);
        }
        return {};
    }

    T* operator->() const noexcept { return m_ptr; }
    T& operator*() const noexcept { return *m_ptr; }
    T* Get() const noexcept { return m_ptr; }
    explicit operator bool() const noexcept { return m_ptr != nullptr; }
};

template<typename T, typename... Args>
KUniquePtr<T> CreateUniquePtr(Args&&... args)
{
    return KUniquePtr<T>(new T(std::forward<Args>(args)...));
}

END_NAMESPACE_CORE
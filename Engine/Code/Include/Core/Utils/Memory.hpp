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

    virtual unsigned long AddRef() = 0;
    virtual unsigned long Release() = 0;
    virtual unsigned long GetRefCount() = 0;
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

    void Release() {
        if (m_ptr) {
            T* temp = m_ptr;
            m_ptr = nullptr;
            temp->Release();
        }
    }

    void Attach(T* p) {
        Release();
        m_ptr = p;
    }

    T* Detach() noexcept {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    template<typename CastType>
    KSharedPtr<CastType> CastAs() const {
        if (!m_ptr) return {};
        CastType* casted = dynamic_cast<CastType*>(m_ptr);
        return casted ? KSharedPtr<CastType>(casted) : KSharedPtr<CastType>();
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

END_NAMESPACE_CORE
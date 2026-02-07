#pragma once

#include <atomic>
#include <spdlog/spdlog.h>

class IResource
{
public:
    virtual ~IResource() = default;

    virtual unsigned long AddRef() = 0;
    virtual unsigned long Release() = 0;
    virtual unsigned long GetRefCount() = 0;
};

template<typename T>
class RefCounter : virtual public T
{
private:
    std::atomic<unsigned long> m_count{ 0 };

public:
    RefCounter() = default;
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
class RefCountPtr
{
private:
    T* m_ptr = nullptr;

public:
    RefCountPtr() = default;
    RefCountPtr(T* p) : m_ptr(p) { if (m_ptr) m_ptr->AddRef(); }

    RefCountPtr(const RefCountPtr& other) : m_ptr(other.m_ptr) { if (m_ptr) m_ptr->AddRef(); }

    template<typename U>
    RefCountPtr(const RefCountPtr<U>& other) : m_ptr(other.Get()) { if (m_ptr) m_ptr->AddRef(); }

    RefCountPtr& operator=(const RefCountPtr& other) 
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

    ~RefCountPtr() { Release(); }

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

    template<typename CastType>
    RefCountPtr<CastType> CastAs() const {
        if (!m_ptr) return {};
        CastType* casted = dynamic_cast<CastType*>(m_ptr);
        return casted ? RefCountPtr<CastType>(casted) : RefCountPtr<CastType>();
    }

    T* operator->() const { return m_ptr; }
    T* Get() const { return m_ptr; }
    explicit operator bool() const { return m_ptr != nullptr; }
};

template<typename T, typename... Args>
RefCountPtr<T> CreateRefPtr(Args&&... args)
{
    return RefCountPtr<T>(new RefCounter<T>(std::forward<Args>(args)...));
}
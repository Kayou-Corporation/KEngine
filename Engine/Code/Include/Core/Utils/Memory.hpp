#pragma once

#include <atomic>

class IResource
{
protected:
	virtual ~IResource() = default;

public:
	virtual unsigned long AddRef() = 0;
	virtual unsigned long Release() = 0;
	virtual unsigned long GetRefCount() = 0;
};

template<typename T>
class RefCounter : public T
{
private:
	std::atomic<unsigned long> m_refCount{ 1 };

public:
	unsigned long AddRef() override
	{
		return ++m_refCount;
	}

	unsigned long Release() override
	{
		auto count = --m_refCount;
		if (count == 0)
			delete this;
		return count;
	}

	unsigned long GetRefCount() override
	{
		return m_refCount.load();
	}
};

template<typename T>
class RefCountPtr
{
private:
	T* ptr = nullptr;

public:
	RefCountPtr() = default;

	RefCountPtr(T* p) : ptr(p)
	{
		if (ptr)
			ptr->AddRef();
	}

	RefCountPtr(const RefCountPtr& other) : ptr(other.ptr)
	{
		if (ptr)
			ptr->AddRef();
	}

	template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
	RefCountPtr(const RefCountPtr<U>& other) : ptr(other.Get())
	{
		if (ptr)
			ptr->AddRef();
	}

	~RefCountPtr()
	{
		if (ptr)
			ptr->Release();
	}

	void Attach(T* p)
	{
		if (ptr)
			ptr->Release();
		ptr = p;
	}

	template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
	void Attach(U* p)
	{
		if (ptr)
			ptr->Release();
		ptr = p;
	}

	template<typename CastType>
	RefCountPtr<CastType> CastAs() const
	{
		static_assert(std::is_base_of_v<IResource, CastType>, "CastAs<CastType>: CastType must inherit from IResource");

		static_assert(!std::is_abstract_v<CastType>, "CastAs<CastType>: CastType cannot be abstract class");

		if (!ptr)
			return {};

		CastType* casted = dynamic_cast<CastType*>(ptr);
		if (!casted)
			return {};

		return RefCountPtr<CastType>(casted);
	}

	template<typename CastType>
	RefCountPtr<CastType> UnsafeCastAs() const
	{
		static_assert(std::is_base_of_v<IResource, CastType>, "CastAs<CastType>: CastType must inherit from IResource");

		//static_assert(!std::is_abstract_v<CastType>, "CastAs<CastType>: CastType cannot be abstract class");

		if (!ptr)
			return {};

		CastType* casted = dynamic_cast<CastType*>(ptr);
		if (!casted)
			return {};

		return RefCountPtr<CastType>(casted);
	}


	T* operator->() const { return ptr; }
	operator bool() const { return ptr != nullptr; }
	T* Get() const { return ptr; }
};

template<typename T, typename... Args>
RefCountPtr<T> CreateRefPtr(Args&&... args)
{
	static_assert(std::is_base_of_v<IResource, T>, "CreateRefPtr<T>: T must inherit from IResource");

	RefCountPtr<T> ref;
	ref.Attach(new T(std::forward<Args>(args)...));
	return ref;
}
#pragma once

#include "Saffron/Core/Managed.h"

namespace Se
{
template <typename T>
class Shared
{
public:
	Shared() :
		_instance(nullptr)
	{
	}

	Shared(std::nullptr_t n) :
		_instance(nullptr)
	{
	}

	Shared(T* instance) :
		_instance(instance)
	{
		static_assert(std::is_base_of<Managed, T>::value, "Class is not Managed!");

		IncRef();
	}

	template <typename T2>
	Shared(const Shared<T2>& other)
	{
		_instance = static_cast<T*>(other._instance);
		IncRef();
	}

	template <typename T2>
	Shared(Shared<T2>&& other)
	{
		_instance = static_cast<T*>(other._instance);
		other._instance = nullptr;
	}

	~Shared()
	{
		DecRef();
	}

	Shared(const Shared<T>& other) :
		_instance(other._instance)
	{
		IncRef();
	}

	Shared& operator=(std::nullptr_t)
	{
		DecRef();
		_instance = nullptr;
		return *this;
	}

	Shared& operator=(const Shared<T>& other)
	{
		other.IncRef();
		DecRef();

		_instance = other._instance;
		return *this;
	}

	template <typename T2>
	Shared& operator=(const Shared<T2>& other)
	{
		other.IncRef();
		DecRef();

		_instance = other._instance;
		return *this;
	}

	template <typename T2>
	Shared& operator=(Shared<T2>&& other)
	{
		DecRef();

		_instance = other._instance;
		other._instance = nullptr;
		return *this;
	}

	operator bool() { return _instance != nullptr; }

	operator bool() const { return _instance != nullptr; }

	T* operator->() { return _instance; }

	const T* operator->() const { return _instance; }

	T& operator*() { return *_instance; }

	const T& operator*() const { return *_instance; }

	T* Raw() { return _instance; }

	const T* Raw() const { return _instance; }

	void Reset(T* instance = nullptr)
	{
		DecRef();
		_instance = instance;
	}

	template <typename... Args>
	static Shared<T> Create(Args&&... args)
	{
		return Shared<T>(new T(std::forward<Args>(args)...));
	}

private:
	void IncRef() const
	{
		if (_instance) _instance->IncRefCount();
	}

	void DecRef() const
	{
		if (_instance)
		{
			_instance->DecRefCount();
			if (_instance->GetRefCount() == 0)
			{
				delete _instance;
			}
		}
	}

	template <class T2>
	friend class Shared;
	T* _instance;
};
}

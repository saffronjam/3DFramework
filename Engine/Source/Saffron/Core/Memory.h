﻿#pragma once

#include "Saffron/Core/Managed.h"

namespace Se
{
template <typename T>
class Shared
{
public:
	Shared() :
		m_Instance(nullptr)
	{
	}

	Shared(std::nullptr_t n) :
		m_Instance(nullptr)
	{
	}

	Shared(T* instance) :
		m_Instance(instance)
	{
		static_assert(std::is_base_of<Managed, T>::value, "Class is not Managed!");

		IncRef();
	}

	template <typename T2>
	Shared(const Shared<T2>& other)
	{
		m_Instance = (T*)other.m_Instance;
		IncRef();
	}

	template <typename T2>
	Shared(Shared<T2>&& other)
	{
		m_Instance = (T*)other.m_Instance;
		other.m_Instance = nullptr;
	}

	~Shared()
	{
		DecRef();
	}

	Shared(const Shared<T>& other) :
		m_Instance(other.m_Instance)
	{
		IncRef();
	}

	Shared& operator=(std::nullptr_t)
	{
		DecRef();
		m_Instance = nullptr;
		return *this;
	}

	Shared& operator=(const Shared<T>& other)
	{
		other.IncRef();
		DecRef();

		m_Instance = other.m_Instance;
		return *this;
	}

	template <typename T2>
	Shared& operator=(const Shared<T2>& other)
	{
		other.IncRef();
		DecRef();

		m_Instance = other.m_Instance;
		return *this;
	}

	template <typename T2>
	Shared& operator=(Shared<T2>&& other)
	{
		DecRef();

		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }

	operator bool() const { return m_Instance != nullptr; }

	T* operator->() { return m_Instance; }

	const T* operator->() const { return m_Instance; }

	T& operator*() { return *m_Instance; }

	const T& operator*() const { return *m_Instance; }

	T* Raw() { return m_Instance; }

	const T* Raw() const { return m_Instance; }

	void Reset(T* instance = nullptr)
	{
		DecRef();
		m_Instance = instance;
	}

	template <typename... Args>
	static Shared<T> Create(Args&&... args)
	{
		return Shared<T>(new T(std::forward<Args>(args)...));
	}

private:
	void IncRef() const
	{
		if (m_Instance) m_Instance->IncRefCount();
	}

	void DecRef() const
	{
		if (m_Instance)
		{
			m_Instance->DecRefCount();
			if (m_Instance->GetRefCount() == 0)
			{
				delete m_Instance;
			}
		}
	}

	template <class T2>
	friend class Shared;
	T* m_Instance;
};
}

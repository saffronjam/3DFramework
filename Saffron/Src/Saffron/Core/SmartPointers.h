#pragma once

#include <atomic>

#include "Saffron/System/TypeDefs.h"

namespace Se
{
class ReferenceCounted
{
public:
	void IncreaseReferenceCount() const
	{
		++m_ReferenceCount;
	}
	void DecreaseReferenceCount() const
	{
		--m_ReferenceCount;
	}

	Uint32 GetReferenceCount() const { return m_ReferenceCount; }
private:
	mutable std::atomic<Uint32> m_ReferenceCount = 0;
};

template<typename T>
class Shared
{
public:
	Shared()
		: m_Instance(nullptr)
	{
	}

	Shared(std::nullptr_t n)
		: m_Instance(nullptr)
	{
	}

	Shared(T *instance)
		: m_Instance(instance)
	{
		static_assert(std::is_base_of<ReferenceCounted, T>::value, "Class is not ReferenceCounted!");
		IncreaseReferenceCount();
	}

	template<typename T2>
	Shared(const Shared<T2> &other)
	{
		m_Instance = static_cast<T *>(other.m_Instance);
		IncreaseReferenceCount();
	}

	template<typename T2>
	Shared(Shared<T2> &&other)
	{
		m_Instance = static_cast<T *>(other.m_Instance);
		other.m_Instance = nullptr;
	}

	~Shared()
	{
		DecreaseReferenceCount();
	}

	Shared(const Shared<T> &other)
		: m_Instance(other.m_Instance)
	{
		IncreaseReferenceCount();
	}

	Shared &operator=(std::nullptr_t)
	{
		DecreaseReferenceCount();
		m_Instance = nullptr;
		return *this;
	}

	Shared &operator=(const Shared<T> &other)
	{
		other.IncreaseReferenceCount();
		DecreaseReferenceCount();

		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename T2>
	Shared &operator=(const Shared<T2> &other)
	{
		other.IncreaseReferenceCount();
		DecreaseReferenceCount();

		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename T2>
	Shared &operator=(Shared<T2> &&other)
	{
		DecreaseReferenceCount();

		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }
	operator bool() const { return m_Instance != nullptr; }

	T *operator->() { return m_Instance; }
	const T *operator->() const { return m_Instance; }

	T &operator*() { return *m_Instance; }
	const T &operator*() const { return *m_Instance; }

	T *Raw() { return  m_Instance; }
	const T *Raw() const { return  m_Instance; }

	void Reset(T *instance = nullptr)
	{
		DecreaseReferenceCount();
		m_Instance = instance;
		IncreaseReferenceCount();
	}

	template<typename... Args>
	static Shared<T> Create(Args&&... args)
	{
		return Shared<T>(new T(std::forward<Args>(args)...));
	}

private:
	void IncreaseReferenceCount() const
	{
		if ( m_Instance )
		{
			m_Instance->IncreaseReferenceCount();
		}
	}

	void DecreaseReferenceCount() const
	{
		if ( m_Instance )
		{
			m_Instance->DecreaseReferenceCount();
			if ( m_Instance->GetReferenceCount() == 0 )
			{
				delete m_Instance;
			}
		}
	}

	template<class T2>
	friend class Shared;
	T *m_Instance;
};

template<typename T>
class Weak
{
public:
	Weak()
		: m_Instance(nullptr)
	{
	}

	Weak(std::nullptr_t n)
		: m_Instance(nullptr)
	{
	}

	Weak(T *instance)
		: m_Instance(instance)
	{
		static_assert(std::is_base_of<ReferenceCounted, T>::value, "Class is not ReferenceCounted!");
	}

	template<typename T2>
	Weak(const Weak<T2> &other)
		: m_Instance(static_cast<T *>(other.m_Instance))
	{
	}

	template<typename T2>
	Weak(Weak<T2> &&other)
	{
		m_Instance = static_cast<T *>(other.m_Instance);
		other.m_Instance = nullptr;
	}

	~Weak()
	{
	}

	Weak(const Weak<T> &other)
		: m_Instance(other.m_Instance)
	{
	}

	Weak &operator=(std::nullptr_t)
	{
		m_Instance = nullptr;
		return *this;
	}

	Weak &operator=(const Weak<T> &other)
	{
		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename T2>
	Weak &operator=(const Weak<T2> &other)
	{
		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename T2>
	Weak &operator=(Weak<T2> &&other)
	{
		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }
	operator bool() const { return m_Instance != nullptr; }

	T *operator->() { return m_Instance; }
	const T *operator->() const { return m_Instance; }

	T &operator*() { return *m_Instance; }
	const T &operator*() const { return *m_Instance; }

	T *Raw() { return  m_Instance; }
	const T *Raw() const { return  m_Instance; }

	void Reset(T *instance = nullptr)
	{
		m_Instance = instance;
	}

	template<typename... Args>
	static Weak<T> Create(Args&&... args)
	{
		return Weak<T>(new T(std::forward<Args>(args)...));
	}

	template<class T2>
	friend class Weak;
	T *m_Instance;
};

template<typename T>
class Unique
{
public:
	Unique()
		: m_Instance(nullptr)
	{
	}

	Unique(std::nullptr_t n)
		: m_Instance(nullptr)
	{
	}

	Unique(T *instance)
		: m_Instance(instance)
	{
		static_assert(std::is_base_of<ReferenceCounted, T>::value, "Class is not ReferenceCounted!");
		Set();
	}

	template<typename T2>
	Unique(const Unique<T2> &other) = delete;

	template<typename T2>
	Unique(Unique<T2> &&other)
	{
		m_Instance = static_cast<T *>(other.m_Instance);
		other.m_Instance = nullptr;
	}

	~Unique()
	{
		Unset();
	}

	Unique(const Unique<T> &other) = delete;

	Unique &operator=(std::nullptr_t)
	{
		Unset();
		m_Instance = nullptr;
		return *this;
	}

	Unique &operator=(const Unique<T> &other) = delete;

	template<typename T2>
	Unique &operator=(const Unique<T2> &other) = delete;

	template<typename T2>
	Unique &operator=(Unique<T2> &&other)
	{
		Unset();
		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }
	operator bool() const { return m_Instance != nullptr; }

	T *operator->() { return m_Instance; }
	const T *operator->() const { return m_Instance; }

	T &operator*() { return *m_Instance; }
	const T &operator*() const { return *m_Instance; }

	T *Raw() { return  m_Instance; }
	const T *Raw() const { return  m_Instance; }

	void Reset(T *instance = nullptr)
	{
		Unset();
		m_Instance = instance;
		Set();
	}

	template<typename... Args>
	static Unique<T> Create(Args&&... args)
	{
		return Unique<T>(new T(std::forward<Args>(args)...));
	}

private:
	void Set() const
	{
		if ( m_Instance )
		{
			m_Instance->IncreaseReferenceCount();
		}
	}

	void Unset() const
	{
		if ( m_Instance )
		{
			m_Instance->DecreaseReferenceCount();
			if ( m_Instance->GetReferenceCount() == 0 )
			{
				delete m_Instance;
			}
		}
	}

	template<class T2>
	friend class Unique;
	T *m_Instance;
};

}
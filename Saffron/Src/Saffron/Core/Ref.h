#pragma once

#include "Saffron/System/TypeDefs.h"

namespace Se
{
class RefCounted
{
public:
	RefCounted();
	virtual ~RefCounted() = default;

	void IncreaseReferenceCount() const;
	void DecreaseReferenceCount() const;

	Uint32 GetReferenceCount() const { return m_ReferenceCount; }

private:
	mutable Uint32 m_ReferenceCount;
};

template<typename T>
class Ref
{
public:
	Ref();
	Ref(std::nullptr_t n);
	Ref(T *instance);
	template<typename T2>
	Ref(const Ref<T2> &other);
	template<typename T2>
	Ref(Ref<T2> &&other);
	Ref(const Ref<T> &other);
	Ref &operator=(std::nullptr_t);
	Ref &operator=(const Ref<T> &other);
	template<typename T2>
	Ref &operator=(const Ref<T2> &other);
	template<typename T2>
	Ref &operator=(Ref<T2> &&other);
	~Ref() = default;

	operator bool();
	operator bool() const;

	T *operator->();
	const T *operator->() const;
	T &operator*();
	const T &operator*() const;

	T *Raw() { return  m_Instance; }
	const T *Raw() const { return  m_Instance; }

	void Reset(T *instance = nullptr);

	template<typename... Args>
	static Ref<T> Create(Args&&... args);

private:
	void Inc() const;
	void Dec() const;

private:
	template<class T2>
	friend class Ref;
	T *m_Instance;

};

template <typename T>
Ref<T>::Ref() : m_Instance(nullptr)
{
}

template <typename T>
Ref<T>::Ref(std::nullptr_t n) : m_Instance(nullptr)
{
}

template <typename T>
Ref<T>::Ref(T *instance) : m_Instance(instance)
{
	static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted");

	Inc();
}

template <typename T>
template <typename T2>
Ref<T>::Ref(const Ref<T2> &other) : m_Instance(static_cast<T *>(other.m_Instance))
{
	Inc();
}

template <typename T>
template <typename T2>
Ref<T>::Ref(Ref<T2> &&other) : m_Instance(static_cast<T *>(other.m_Instance))
{
	other.m_Instance = nullptr;
}

template <typename T>
Ref<T>::Ref(const Ref<T> &other) : m_Instance(other.m_Instance)
{
	Inc();
}

template <typename T>
Ref<T> &Ref<T>::operator=(std::nullptr_t)
{
	Dec();
	m_Instance = nullptr;
	return *this;
}

template <typename T>
Ref<T> &Ref<T>::operator=(const Ref<T> &other)
{
	other.Inc();
	Dec();

	m_Instance = other.m_Instance;
	return *this;
}

template <typename T>
template <typename T2>
Ref<T> &Ref<T>::operator=(const Ref<T2> &other)
{
	other.IncRef();
	Dec();

	m_Instance = other.m_Instance;
	return *this;
}

template <typename T>
template <typename T2>
Ref<T> &Ref<T>::operator=(Ref<T2> &&other)
{
	Dec();

	m_Instance = other.m_Instance;
	other.m_Instance = nullptr;
	return *this;
}

template <typename T>
Ref<T>::operator bool()
{
	return m_Instance != nullptr;
}

template <typename T>
Ref<T>::operator bool() const
{
	return m_Instance != nullptr;
}

template <typename T>
T *Ref<T>::operator->()
{
	return m_Instance;
}

template <typename T>
const T *Ref<T>::operator->() const
{
	return m_Instance;
}

template <typename T>
T &Ref<T>::operator*()
{
	return *m_Instance;
}

template <typename T>
const T &Ref<T>::operator*() const
{
	return *m_Instance;
}

template <typename T>
void Ref<T>::Reset(T *instance)
{
	Dec();
	m_Instance = instance;
}

template <typename T>
template <typename ... Args>
Ref<T> Ref<T>::Create(Args&&... args)
{
	return Ref<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
void Ref<T>::Inc() const
{
	if ( m_Instance )
		m_Instance->IncreaseReferenceCount();
}

template <typename T>
void Ref<T>::Dec() const
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
}


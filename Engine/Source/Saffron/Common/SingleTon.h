#pragma once

#include "Saffron/Core/NonCopyable.h"
#include "Saffron/Debug/Debug.h"

namespace Se
{
template <class Class>
class SingleTon : public NonCopyable
{
public:
	explicit SingleTon(Class* instance);
	virtual ~SingleTon();

	static auto Instance() -> Class&;

protected:
	static Class* _instance;
};

template <class Class>
SingleTon<Class>::SingleTon(Class* instance)
{
	Debug::Assert(_instance == nullptr, String(typeid(Class).name()) + " was already instansiated");
	_instance = instance;
}

template <class Class>
SingleTon<Class>::~SingleTon()
{
	_instance = nullptr;
}

template <class Class>
auto SingleTon<Class>::Instance() -> Class&
{
	Debug::Assert(_instance != nullptr, String(typeid(Class).name()) + " was not instansiated");
	return *_instance;
}

template <class Class>
Class* SingleTon<Class>::_instance = nullptr;
}

#pragma once

#include "Saffron/Common/NonCopyable.h"
#include "Saffron/Debug/Debug.h"

namespace Se
{
template <class Class>
class Singleton : public NonCopyable
{
public:
	explicit Singleton(Class* instance);
	virtual ~Singleton();

	static auto Instance() -> Class&;

protected:
	static Class* _instance;
};

template <class Class>
Singleton<Class>::Singleton(Class* instance)
{
	Debug::Assert(_instance == nullptr, std::string(typeid(Class).name()) + " was already instansiated");
	_instance = instance;
}

template <class Class>
Singleton<Class>::~Singleton()
{
	_instance = nullptr;
}

template <class Class>
auto Singleton<Class>::Instance() -> Class&
{
	Debug::Assert(_instance != nullptr, std::string(typeid(Class).name()) + " was not instansiated");
	return *_instance;
}

template <class Class>
Class* Singleton<Class>::_instance = nullptr;
}

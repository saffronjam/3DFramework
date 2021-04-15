#pragma once

#include "Saffron/Base.h"

namespace Se
{
template <class Class>
class Instansiated
{
public:
	explicit Instansiated(Class* instance);
	~Instansiated();

	static Class& Instance();

protected:
	static Class* _instance;
};

template <class Class>
Instansiated<Class>::Instansiated(Class* instance)
{
	SE_CORE_ASSERT(_instance == nullptr, String(typeid(Class).name()) + " was already instansiated");
	_instance = instance;
}

template <class Class>
Instansiated<Class>::~Instansiated()
{
	_instance = nullptr;
}

template <class Class>
Class& Instansiated<Class>::Instance()
{
	SE_CORE_ASSERT(_instance != nullptr, String(typeid(Class).name()) + " was not instansiated");
	return *_instance;
}

template <class Class>
Class* Instansiated<Class>::_instance = nullptr;
}

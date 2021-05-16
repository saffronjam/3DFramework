#pragma once

namespace Se
{
template <class Class>
class SingleTon : public NonCopyable
{
public:
	explicit SingleTon(Class* instance);
	~SingleTon();

	static Class& Instance();

protected:
	static Class* _instance;
};

template <class Class>
SingleTon<Class>::SingleTon(Class* instance)
{
	Debug::Assert(_instance == nullptr, String(typeid(Class).name()) + " was already instansiated");;
	_instance = instance;
}

template <class Class>
SingleTon<Class>::~SingleTon()
{
	_instance = nullptr;
}

template <class Class>
Class& SingleTon<Class>::Instance()
{
	Debug::Assert(_instance != nullptr, String(typeid(Class).name()) + " was not instansiated");;
	return *_instance;
}

template <class Class>
Class* SingleTon<Class>::_instance = nullptr;
}

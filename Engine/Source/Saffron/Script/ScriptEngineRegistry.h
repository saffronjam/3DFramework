#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/GenUtils.h"

namespace Se
{
class ScriptEngineRegistry : public SingleTon<ScriptEngineRegistry>
{
public:
	ScriptEngineRegistry();

	static void RegisterAll();

	static bool ExecuteHasComponent(MonoType* type, Entity& entity);
	static void ExecuteCreateComponent(MonoType* type, Entity& entity);

private:
	void RegisterComponents();
	template <class ComponentClass>
	void RegisterComponent();

private:
	HashMap<MonoType*, Function<bool(Entity&)>> _hasComponentFuncs;
	HashMap<MonoType*, Function<void(Entity&)>> _createComponentFuncs;
};

template <class ComponentClass>
void ScriptEngineRegistry::RegisterComponent()
{
	auto name = GenUtils::NamespaceAndClassString<ComponentClass>();
	MonoType* type = mono_reflection_type_from_name(const_cast<char*>(("Se." + name.second).c_str()),
	                                                ScriptEngine::Instance().GetCoreImage());
	if (type)
	{
		uint id = mono_type_get_type(type);
		_hasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<TagComponent>(); };
		_createComponentFuncs[type] = [](Entity& entity) { entity.AddComponent<TagComponent>(); };
	}
	else
	{
		Log::CoreError("No C# component class found for " "TagComponent" "!");
	}
}
}

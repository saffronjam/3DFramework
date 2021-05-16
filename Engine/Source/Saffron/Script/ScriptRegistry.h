#pragma once

#include <monopp/Type.h>

#include "Saffron/Core/GenUtils.h"
#include "Saffron/Core/SingleTon.h"
#include "Saffron/Core/TypeDefs.h"
#include "Saffron/Script/ScriptEngine.h"

#include <any>

namespace Se
{
class Entity;

class ScriptRegistry : public SingleTon<ScriptRegistry>
{
public:
	ScriptRegistry();

	void RegisterAll();

	auto HasComponent(const Mono::Type& type, Entity& entity) -> bool;
	void CreateComponent(const Mono::Type& type, Entity& entity);

private:
	void RegisterComponents();

	template <class ComponentClass>
	void RegisterComponent()
	{
		auto name = GenUtils::NamespaceAndClassString<ComponentClass>();
		auto type = ScriptEngine::Instance().GetCoreAssembly().GetType("Saffron", name.second);

		if (type.Valid())
		{
			_hasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<TagComponent>(); };
			_createComponentFuncs[type] = [](Entity& entity) { entity.AddComponent<TagComponent>(); };
		}
		else
		{
			Log::CoreError(String("No C# component class found for ") + typeid(ComponentClass).name() + "!");
		}
	}


private:
	HashMap<String, std::any> _registry;


	struct MonoTypeHasher
	{
	public:
		auto operator()(const Mono::Type& type) const -> size_t
		{
			return std::hash<std::string>()(type.GetFullname());
		}
	};

	std::unordered_map<Mono::Type, Function<bool(Entity&)>, MonoTypeHasher> _hasComponentFuncs;
	std::unordered_map<Mono::Type, Function<void(Entity&)>, MonoTypeHasher> _createComponentFuncs;
};
}

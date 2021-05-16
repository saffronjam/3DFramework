#pragma once

#include <monopp/Method.h>
#include <monopp/GcHandle.h>
#include <monopp/Property.h>

#include "Saffron/Base.h"
#include "Saffron/Script/Field.h"

namespace Se
{
class Scene;

class ScriptEntity
{
public:
	ScriptEntity(Mono::Type monoType, Mono::Object monoObject, Scene& scene);
	ScriptEntity(Mono::Type monoType, Scene& scene);
	ScriptEntity(const ScriptEntity&) = delete;
	ScriptEntity(ScriptEntity&& other) noexcept;
	auto operator=(const ScriptEntity&) -> ScriptEntity = delete;
	auto operator=(ScriptEntity&&) -> ScriptEntity = delete;

	void CreateNewObject();

	auto GetMonoObject() const -> const Mono::Object&;
	auto GetMonoType() const -> const Mono::Type&;

	auto Valid() const -> bool;

	void SetScript(String moduleName);

private:
	static Mono::Method OnCreateMethod;
	static Mono::Method OnDestroyMethod;
	static Mono::Method OnUpdateMethod;

	static Mono::Property IdProperty;

	static Mono::Method OnCollision2DBeginMethod;
	static Mono::Method OnCollision2DEndMethod;
	static Mono::Method OnCollision3DBeginMethod;
	static Mono::Method OnCollision3DEndMethod;

private:
	Mono::Type _monoType; // Entity-class, or derived from entity-class
	Mono::GcHandle _monoObject;
	Mono::ScopedGcHandle _scopedGcHandle;
	Scene& _scene;

	friend class ScriptEngine;
};
}

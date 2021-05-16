#include "SaffronPCH.h"

#include "Saffron/Script/ScriptEntity.h"

#include "ScriptEngine.h"

namespace Se
{
Mono::Method ScriptEntity::OnCreateMethod;
Mono::Method ScriptEntity::OnDestroyMethod;
Mono::Method ScriptEntity::OnUpdateMethod;

Mono::Property ScriptEntity::IdProperty;

Mono::Method ScriptEntity::OnCollision2DBeginMethod;
Mono::Method ScriptEntity::OnCollision2DEndMethod;
Mono::Method ScriptEntity::OnCollision3DBeginMethod;
Mono::Method ScriptEntity::OnCollision3DEndMethod;

ScriptEntity::ScriptEntity(Mono::Type monoType, Mono::Object monoObject, Scene& scene):
	_monoType(Move(monoType)),
	_monoObject(Move(monoObject)),
	_scopedGcHandle(_monoObject),
	_scene(scene)
{
}

ScriptEntity::ScriptEntity(Mono::Type monoType, Scene& scene):
	_monoType(Move(monoType)),
	_monoObject(_monoType.NewInstance()),
	_scopedGcHandle(_monoObject),
	_scene(scene)
{
}

ScriptEntity::ScriptEntity(ScriptEntity&& other) noexcept :
	_monoType(Move(other._monoType)),
	_monoObject(Move(other._monoObject)),
	_scopedGcHandle(_monoObject),
	_scene(other._scene)
{
	other._scopedGcHandle.Invalidate();
}

void ScriptEntity::CreateNewObject()
{
	_monoObject = Move(Mono::GcHandle(_monoType.NewInstance()));
}

auto ScriptEntity::GetMonoObject() const -> const Mono::Object&
{
	return _monoObject;
}

auto ScriptEntity::GetMonoType() const -> const Mono::Type&
{
	return _monoType;
}

auto ScriptEntity::Valid() const -> bool
{
	return _monoObject.Valid() && _monoType.Valid();
}

void ScriptEntity::SetScript(String moduleName)
{
}
}

#pragma once

#include "Saffron/Base.h"
#include "Saffron/Script/MonoApi.h"

namespace Se
{
class Scene;

struct EntityScriptClass
{
	String FullName;
	String ClassName;
	String NamespaceName;

	MonoClass* Class = nullptr;
	MonoMethod* OnCreateMethod = nullptr;
	MonoMethod* OnDestroyMethod = nullptr;
	MonoMethod* OnUpdateMethod = nullptr;

	// Physics
	MonoMethod* OnCollision2DBeginMethod = nullptr;
	MonoMethod* OnCollision2DEndMethod = nullptr;
	MonoMethod* OnCollision3DBeginMethod = nullptr;
	MonoMethod* OnCollision3DEndMethod = nullptr;

	void SyncClassMethods(MonoImage* image);
	void SetScript(String moduleName);
};

struct EntityInstance
{
	EntityScriptClass* ScriptClass = nullptr;

	uint Handle = 0;
	Scene* SceneInstance = nullptr;

	MonoObject* GetInstance() const;
};
}

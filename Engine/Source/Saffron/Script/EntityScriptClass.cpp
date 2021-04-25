#include "SaffronPCH.h"

#include "Saffron/Script/EntityScriptClass.h"

#include "ScriptEngine.h"

namespace Se
{
void EntityScriptClass::SyncClassMethods(MonoImage* image)
{
	OnCreateMethod = MonoApi::GetMethod(image, FullName + ":OnCreate()");
	OnUpdateMethod = MonoApi::GetMethod(image, FullName + ":OnUpdate(single)");

	// Physics (Entity class)
	auto* assembly = const_cast<MonoImage*>(ScriptEngine::Instance().GetCoreImage());
	OnCollision2DBeginMethod = MonoApi::GetMethod(assembly, "Se.Entity:OnCollision2DBegin(single)");
	OnCollision2DEndMethod = MonoApi::GetMethod(assembly, "Se.Entity:OnCollision2DEnd(single)");
	OnCollision3DBeginMethod = MonoApi::GetMethod(assembly, "Se.Entity:OnCollision3DBegin(single)");
	OnCollision3DEndMethod = MonoApi::GetMethod(assembly, "Se.Entity:OnCollision3DEnd(single)");
}

void EntityScriptClass::SetScript(String moduleName)
{
	FullName = Move(moduleName);
	if (FullName.find('.') != String::npos)
	{
		NamespaceName = FullName.substr(0, FullName.find_last_of('.'));
		ClassName = FullName.substr(FullName.find_last_of('.') + 1);
	}
	else
	{
		ClassName = FullName;
	}
}

MonoObject* EntityInstance::GetInstance() const
{
	Debug::Assert(Handle, "Entity has not been instantiated!");
	return MonoApi::GetInstance(Handle);
}

}

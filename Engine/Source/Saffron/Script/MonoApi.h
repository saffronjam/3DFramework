#pragma once

#include "Saffron/Base.h"
#include "Saffron/Script/MonoDeclarations.h"
#include "Saffron/Script/PublicField.h"

namespace Se
{
struct EntityScriptClass;

class MonoApi
{
public:
	static void Initialize(const String& name, const Filepath& assembliesFilepath);
	static void Shutdown(MonoDomain* domain);

	static MonoDomain* CreateDomain(const String& friendlyName);
	static void UnloadDomain(MonoDomain* domain);
	static void SetActiveDomain(MonoDomain* domain, bool force = false);

	static MonoAssembly* LoadAssembly(const Filepath& filepath);

	static void AttachThread(MonoDomain* domain);
	static void DetachThread();

	static uint Instantiate(MonoDomain* domain, EntityScriptClass& scriptClass);
	static MonoObject* CallMethod(MonoObject* object, MonoMethod* method, void** params = nullptr);

	static MonoString* CreateString(MonoDomain* domain, const String& string);

	static MonoImage* GetImage(MonoAssembly* assembly);

	static MonoClass* GetClass(MonoImage* image, const EntityScriptClass& scriptClass);
	static MonoClass* GetClass(MonoImage* image, const String& namespaceName, const String& className);
	static MonoClass* GetClass(MonoImage* image, const String& fullName);
	static bool IsSubclass(MonoClass* base, MonoClass* derived, bool includeInterface = false);

	static MonoMethod* GetMethod(MonoImage* image, const String& methodDesc);
	static MonoMethod* GetGetter(MonoProperty* property);
	static MonoMethod* GetSetter(MonoProperty* property);
	static MonoProperty* GetProperty(MonoClass* monoClass, const String& name);
	static MonoObject* GetInstance(uint handle);

	static MonoClassField* GetFields(MonoClass* monoClass, void** out);
	static String GetFieldName(MonoClassField* field);
	static uint GetFieldFlags(MonoClassField* field);
	static MonoType *GetFieldType(MonoClassField *field);
	
	static void GetFieldValue(MonoObject* object, MonoClassField* field, void* out);
	static void SetFieldValue(MonoObject* object, MonoClassField* field, void* value);

	static bool IsPublic(uint fieldFlag);

	static FieldType GetSaffronPublicFieldType(MonoType* monoType);

	static void PrintClassMethods(MonoClass* monoClass);
	static void PrintClassProperties(MonoClass* monoClass);
};
}

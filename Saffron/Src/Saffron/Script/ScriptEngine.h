#pragma once



#include "Saffron/Base.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Scene/Scene.h"

extern "C" {
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoString MonoString;
}

namespace Se {

///////////////////////////////////////////////////////////////////////////
/// Forwards Declarations
///////////////////////////////////////////////////////////////////////////

struct EntityScriptClass;


///////////////////////////////////////////////////////////////////////////
/// Field Type
///////////////////////////////////////////////////////////////////////////

enum class FieldType
{
	None = 0, Float, Int, UnsignedInt, String, Vec2, Vec3, Vec4
};


///////////////////////////////////////////////////////////////////////////
/// Entity Instance
///////////////////////////////////////////////////////////////////////////

struct EntityInstance
{
	EntityScriptClass *ScriptClass = nullptr;

	Uint32 Handle = 0;
	Scene *SceneInstance = nullptr;

	MonoObject *GetInstance() const;
};


///////////////////////////////////////////////////////////////////////////
/// Public Field
///////////////////////////////////////////////////////////////////////////

// TODO: This needs to somehow work for strings...
struct PublicField
{
	String Name;
	FieldType Type;

	PublicField(String name, FieldType type);
	PublicField(const PublicField &) = delete;
	PublicField(PublicField &&other);
	~PublicField();

	void CopyStoredValueToRuntime() const;
	bool IsRuntimeAvailable() const;

	template<typename T>
	T GetStoredValue() const;
	template<typename T>
	T GetRuntimeValue() const;

	template<typename T>
	void SetStoredValue(T value) const;
	template<typename T>
	void SetRuntimeValue(T value) const;
	void SetStoredValueRaw(void *src) const;

private:
	EntityInstance *m_EntityInstance{};
	MonoClassField *m_MonoClassField{};
	Uint8 *m_StoredValueBuffer = nullptr;

	Uint8 *AllocateBuffer(FieldType type);
	void SetStoredValue_Internal(void *value) const;
	void GetStoredValue_Internal(void *outValue) const;
	void SetRuntimeValue_Internal(void *value) const;
	void GetRuntimeValue_Internal(void *outValue) const;

	friend class ScriptEngine;
};

using ScriptModuleFieldMap = UnorderedMap<String, UnorderedMap<String, PublicField>>;


///////////////////////////////////////////////////////////////////////////
/// Entity Instance Data
///////////////////////////////////////////////////////////////////////////

struct EntityInstanceData
{
	EntityInstance Instance;
	ScriptModuleFieldMap ModuleFieldMap;
};

using EntityInstanceMap = UnorderedMap<UUID, UnorderedMap<UUID, EntityInstanceData>>;


///////////////////////////////////////////////////////////////////////////
/// Script ProjectManager
///////////////////////////////////////////////////////////////////////////

class ScriptEngine
{
public:
	static void Init(String assemblyPath);
	static void Shutdown();

	static void OnUpdate();
	static void OnGuiRender();
	static void OnSceneDestruct(UUID sceneID);

	static void LoadSaffronRuntimeAssembly(const String &path);
	static void ReloadAssembly(const String &path);

	static void SetSceneContext(const Shared<Scene> &scene);
	static const Shared<Scene> &GetCurrentSceneContext();

	static void CopyEntityScriptData(UUID dst, UUID src);

	static void OnCreateEntity(Entity entity);
	static void OnCreateEntity(UUID sceneID, UUID entityID);
	static void OnUpdateEntity(UUID sceneID, UUID entityID, Time ts);

	static void OnCollision2DBegin(Entity entity);
	static void OnCollision2DBegin(UUID sceneID, UUID entityID);
	static void OnCollision2DEnd(Entity entity);
	static void OnCollision2DEnd(UUID sceneID, UUID entityID);

	static void OnCollision3DBegin(Entity entity);
	static void OnCollision3DBegin(UUID sceneID, UUID entityID);
	static void OnCollision3DEnd(Entity entity);
	static void OnCollision3DEnd(UUID sceneID, UUID entityID);

	static void OnScriptComponentDestroyed(UUID sceneID, UUID entityID);

	static bool ModuleExists(const String &moduleName);
	static void InitScriptEntity(Entity entity);
	static void ShutdownScriptEntity(Entity entity, const String &moduleName);
	static void InstantiateEntityClass(Entity entity);

	static EntityInstanceMap &GetEntityInstanceMap();
	static EntityInstanceData &GetEntityInstanceData(UUID sceneID, UUID entityID);

	static MonoString *CreateMonoString(const char *string);

	static void AttachThread();
	static void DetachThread();

};


///////////////////////////////////////////////////////////////////////////
/// Public Field
///////////////////////////////////////////////////////////////////////////

template <typename T>
T PublicField::GetStoredValue() const
{
	T value;
	GetStoredValue_Internal(&value);
	return value;
}

template <typename T>
T PublicField::GetRuntimeValue() const
{
	T value;
	GetRuntimeValue_Internal(&value);
	return value;
}

template <typename T>
void PublicField::SetStoredValue(T value) const
{
	SetStoredValue_Internal(&value);
}
template <typename T>
void PublicField::SetRuntimeValue(T value) const
{
	SetRuntimeValue_Internal(&value);
}

}
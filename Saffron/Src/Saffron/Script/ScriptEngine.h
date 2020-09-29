#pragma once

#include <string>

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
	std::string Name;
	FieldType Type;

	PublicField(std::string name, FieldType type);
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

using ScriptModuleFieldMap = std::unordered_map<std::string, std::unordered_map<std::string, PublicField>>;


///////////////////////////////////////////////////////////////////////////
/// Entity Instance Data
///////////////////////////////////////////////////////////////////////////

struct EntityInstanceData
{
	EntityInstance Instance;
	ScriptModuleFieldMap ModuleFieldMap;
};

using EntityInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, EntityInstanceData>>;


///////////////////////////////////////////////////////////////////////////
/// Script Engine
///////////////////////////////////////////////////////////////////////////

class ScriptEngine
{
public:
	static void Init(const std::string &assemblyPath);
	static void Shutdown();

	static void OnSceneDestruct(UUID sceneID);

	static void LoadSaffronRuntimeAssembly(const std::string &path);
	static void ReloadAssembly(const std::string &path);

	static void SetSceneContext(const Ref<Scene> &scene);
	static const Ref<Scene> &GetCurrentSceneContext();

	static void CopyEntityScriptData(UUID dst, UUID src);

	static void OnCreateEntity(Entity entity);
	static void OnCreateEntity(UUID sceneID, UUID entityID);
	static void OnUpdateEntity(UUID sceneID, UUID entityID, Time ts);

	static void OnCollision2DBegin(Entity entity);
	static void OnCollision2DBegin(UUID sceneID, UUID entityID);
	static void OnCollision2DEnd(Entity entity);
	static void OnCollision2DEnd(UUID sceneID, UUID entityID);

	static void OnScriptComponentDestroyed(UUID sceneID, UUID entityID);

	static bool ModuleExists(const std::string &moduleName);
	static void InitScriptEntity(Entity entity);
	static void ShutdownScriptEntity(Entity entity, const std::string &moduleName);
	static void InstantiateEntityClass(Entity entity);

	static EntityInstanceMap &GetEntityInstanceMap();
	static EntityInstanceData &GetEntityInstanceData(UUID sceneID, UUID entityID);

	static MonoString *CreateMonoString(const char *string);

	// Debug
	static void OnImGuiRender();
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
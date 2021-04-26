#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Project.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Script/EntityScriptClass.h"
#include "Saffron/Script/ScriptEngineRegistry.h"

namespace Se
{

using EntityInstanceMap = HashMap<UUID, HashMap<UUID, EntityInstance>>;

class ScriptEngine : public SingleTon<ScriptEngine>
{
public:
	ScriptEngine();
	~ScriptEngine();

	static void OnUpdate();
	static void OnGuiRender();
	static void OnProjectChange(const Shared<Project>& project);
	static void OnSceneChange(const Shared<Scene>& scene);
	 
	static void LoadRuntimeAssembly(const Filepath& assemblyFilepath);
	static void ReloadAssembly(const Filepath& assemblyFilepath);

	static void SetSceneContext(const Shared<Scene>& scene);
	static const Shared<Scene>& GetSceneContext();

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

	static bool ModuleExists(const String& moduleName);
	
	static void CreateScriptEntity(Entity entity);
	static void DeleteScriptEntity(Entity entity, const String& moduleName);
	static void InstantiateScriptEntity(Entity entity);

	static EntityInstanceMap& GetEntityInstanceMap();
	static EntityInstance& GetEntityInstance(UUID sceneID, UUID entityID);

	static MonoString *CreateMonoString(const String& string);
	
	void AttachThread();
	void DetachThread();

	MonoImage* GetCoreImage() const;
	MonoImage* GetAppImage() const;

	MonoAssembly* GetCoreAssembly() const;
	MonoAssembly* GetAppAssembly() const;

private:
	MonoClass *GetClass(MonoImage *image, const EntityScriptClass& scriptClass);
	MonoClass *GetClass(MonoImage *image, const String &namespaceName, const String &className);

public:
	static constexpr const char* MonoPath = "../Engine/ThirdParty/mono/bin/Runtime";

private:
	Unique<ScriptEngineRegistry> _scriptEngineRegistry;
	
	MonoDomain* _monoDomain = nullptr;
	Filepath _assemblyFilepath;
	Shared<Project> _projectContext;
	Shared<Scene> _sceneContext;

	MonoImage* _coreImage = nullptr;
	MonoImage* _appImage = nullptr;

	MonoAssembly* _coreAssembly = nullptr;
	MonoAssembly* _appAssembly = nullptr;

	EntityInstanceMap _entityInstanceMap;
	TreeMap<String, MonoClass*> _classCacheMap;
	HashMap<String, EntityScriptClass> _entityClassMap;

	static constexpr const char* ScriptsPath = "Assets/Scripts/";
	static constexpr const char* ScriptsExtension = ".dll";

	static constexpr const char* FriendlyDomainName = "Saffron Runtime";
	static constexpr const char* ScriptCoreScript = "Assets/Scripts/ScriptCore.dll";
};
}

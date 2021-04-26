#include "SaffronPCH.h"

#include "Saffron/Entity/Entity.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Script/MonoApi.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/Script/ScriptEngineRegistry.h"

namespace Se
{
ScriptEngine::ScriptEngine() :
	SingleTon(this),
	_scriptEngineRegistry(CreateUnique<ScriptEngineRegistry>())
{
	MonoApi::Initialize("Saffron", MonoPath);
	_monoDomain = MonoApi::CreateDomain(FriendlyDomainName);
}

ScriptEngine::~ScriptEngine()
{
	_sceneContext = nullptr;
	_entityInstanceMap.clear();
	MonoApi::Shutdown(_monoDomain);
}

void ScriptEngine::OnUpdate()
{
}

void ScriptEngine::OnGuiRender()
{
	ImGui::Begin("Script Engine Debug");
	for (auto& [sceneID, entityMap] : Instance()._entityInstanceMap)
	{
		bool opened = ImGui::TreeNode(reinterpret_cast<void*>(static_cast<ulong>(sceneID)), "Scene (%llx)",
		                              static_cast<ulong>(sceneID));
		if (opened)
		{
			Shared<Scene> scene = Scene::GetScene(sceneID);
			for (auto& [entityID, entityInstance] : entityMap)
			{
				Entity entity = scene->GetScene(sceneID)->GetEntityMap().at(entityID);
				String entityName = "Unnamed Entity";
				if (entity.HasComponent<TagComponent>()) entityName = entity.GetComponent<TagComponent>().Tag;
				opened = ImGui::TreeNode(reinterpret_cast<void*>(static_cast<ulong>(entityID)), "%s (%llx)",
				                         entityName.c_str(), static_cast<ulong>(entityID));
				if (opened)
				{
					for (auto& [moduleName, fieldMap] : entityInstance.ModuleFieldMap)
					{
						opened = ImGui::TreeNode(moduleName.c_str());
						if (opened)
						{
							for (auto& [fieldName, field] : fieldMap)
							{
								opened = ImGui::TreeNodeEx(static_cast<void*>(&field), ImGuiTreeNodeFlags_Leaf,
								                           fieldName.c_str());
								if (opened)
								{
									ImGui::TreePop();
								}
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void ScriptEngine::OnProjectChange(const Shared<Project>& project)
{
	if (Instance()._projectContext)
	{
		for (const auto& scene : Instance()._projectContext->GetSceneCache())
		{
			auto uuid = scene->GetUUID();
			if (Instance()._entityInstanceMap.find(uuid) != Instance()._entityInstanceMap.end())
			{
				Instance()._entityClassMap.clear();
				Instance()._entityInstanceMap.at(uuid).clear();
				Instance()._entityInstanceMap.erase(uuid);
			}
		}
		Instance()._classCacheMap.clear();
	}
	Instance()._projectContext = project;
	if (project)
	{
		LoadRuntimeAssembly(ScriptsPath + Instance()._projectContext->GetName() + ScriptsExtension);
	}
}

void ScriptEngine::OnSceneChange(const Shared<Scene>& scene)
{
	Instance()._sceneContext = scene;
}

void ScriptEngine::LoadRuntimeAssembly(const Filepath& assemblyFilepath)
{
	MonoDomain* domain = nullptr;
	bool cleanup = false;
	if (Instance()._monoDomain)
	{
		domain = MonoApi::CreateDomain(FriendlyDomainName);
		MonoApi::SetActiveDomain(domain);
		cleanup = true;
	}

	Instance()._coreAssembly = MonoApi::LoadAssembly(ScriptCoreScript);
	Instance()._coreImage = MonoApi::GetImage(Instance()._coreAssembly);

	Instance()._appAssembly = MonoApi::LoadAssembly(assemblyFilepath);
	Instance()._appImage = MonoApi::GetImage(Instance()._appAssembly);

	ScriptEngineRegistry::RegisterAll();

	if (cleanup)
	{
		MonoApi::UnloadDomain(Instance()._monoDomain);
		Instance()._monoDomain = domain;
	}
}

void ScriptEngine::ReloadAssembly(const Filepath& assemblyFilepath)
{
	Instance()._assemblyFilepath = ScriptsPath + assemblyFilepath.string() + ScriptsExtension;
	LoadRuntimeAssembly(Instance()._assemblyFilepath);
	Instance()._classCacheMap.clear();
	if (!Instance()._entityInstanceMap.empty())
	{
		Shared<Scene> scene = GetSceneContext();
		Debug::Assert(scene, "No active scene!");
		if (Instance()._entityInstanceMap.find(scene->GetUUID()) != Instance()._entityInstanceMap.end())
		{
			auto& entityMap = Instance()._entityInstanceMap.at(scene->GetUUID());
			for (auto& [entityID, entityInstance] : entityMap)
			{
				const auto& entityMap = scene->GetEntityMap();
				Debug::Assert(entityMap.find(entityID) != entityMap.end(),
				              "Invalid entity ID or entity doesn't exist in scene!");
				CreateScriptEntity(entityMap.at(entityID));
			}
		}
	}
}

void ScriptEngine::SetSceneContext(const Shared<Scene>& scene)
{
	Instance()._sceneContext = scene;
}

const Shared<Scene>& ScriptEngine::GetSceneContext()
{
	return Instance()._sceneContext;
}

void ScriptEngine::CopyEntityScriptData(UUID dst, UUID src)
{
	auto& sceneMap = GetEntityInstanceMap();

	Debug::Assert(sceneMap.find(dst) != sceneMap.end());
	Debug::Assert(sceneMap.find(src) != sceneMap.end());

	auto& dstEntityMap = sceneMap.at(dst);
	auto& srcEntityMap = sceneMap.at(src);

	for (auto& [entityID, entityInstance] : srcEntityMap)
	{
		for (auto& [moduleName, srcFieldMap] : srcEntityMap[entityID].ModuleFieldMap)
		{
			auto& dstModuleFieldMap = dstEntityMap[entityID].ModuleFieldMap;
			for (auto& [fieldName, field] : srcFieldMap)
			{
				Debug::Assert(dstModuleFieldMap.find(moduleName) != dstModuleFieldMap.end());
				auto& fieldMap = dstModuleFieldMap.at(moduleName);
				Debug::Assert(fieldMap.find(fieldName) != fieldMap.end());
				fieldMap.at(fieldName).SetStoredValue(static_cast<void*>(field._storedValueBuffer));
			}
		}
	}
}

void ScriptEngine::OnCreateEntity(Entity entity)
{
	OnCreateEntity(entity.GetSceneUUID(), entity.GetComponent<IDComponent>().ID);
}

void ScriptEngine::OnCreateEntity(UUID sceneID, UUID entityID)
{
	EntityInstance& entityInstance = GetEntityInstance(sceneID, entityID);
	if (entityInstance.ScriptClass->OnCreateMethod)
	{
		MonoApi::CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnCreateMethod);
	}
}

void ScriptEngine::OnUpdateEntity(UUID sceneID, UUID entityID, Time ts)
{
	EntityInstance& entityInstance = GetEntityInstance(sceneID, entityID);
	if (entityInstance.ScriptClass->OnUpdateMethod)
	{
		void* args[] = {&ts};
		MonoApi::CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnUpdateMethod, args);
	}
}

void ScriptEngine::OnCollision2DBegin(Entity entity)
{
	OnCollision2DBegin(entity.GetSceneUUID(), entity.GetComponent<IDComponent>().ID);
}

void ScriptEngine::OnCollision2DBegin(UUID sceneID, UUID entityID)
{
	EntityInstance& entityInstance = GetEntityInstance(sceneID, entityID);
	if (entityInstance.ScriptClass->OnCollision2DBeginMethod)
	{
		float value = 5.0f;
		void* args[] = {&value};
		MonoApi::CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnCollision2DBeginMethod, args);
	}
}

void ScriptEngine::OnCollision2DEnd(Entity entity)
{
	OnCollision2DEnd(entity.GetSceneUUID(), entity.GetComponent<IDComponent>().ID);
}

void ScriptEngine::OnCollision2DEnd(UUID sceneID, UUID entityID)
{
	EntityInstance& entityInstance = GetEntityInstance(sceneID, entityID);
	if (entityInstance.ScriptClass->OnCollision2DEndMethod)
	{
		float value = 5.0f;
		void* args[] = {&value};
		MonoApi::CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnCollision2DEndMethod, args);
	}
}

void ScriptEngine::OnCollision3DBegin(Entity entity)
{
	OnCollision3DBegin(entity.GetSceneUUID(), entity.GetComponent<IDComponent>().ID);
}

void ScriptEngine::OnCollision3DBegin(UUID sceneID, UUID entityID)
{
	EntityInstance& entityInstance = GetEntityInstance(sceneID, entityID);
	if (entityInstance.ScriptClass->OnCollision3DBeginMethod)
	{
		float value = 5.0f;
		void* args[] = {&value};
		MonoApi::CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnCollision3DBeginMethod, args);
	}
}

void ScriptEngine::OnCollision3DEnd(Entity entity)
{
	OnCollision3DEnd(entity.GetSceneUUID(), entity.GetComponent<IDComponent>().ID);
}

void ScriptEngine::OnCollision3DEnd(UUID sceneID, UUID entityID)
{
	EntityInstance& entityInstance = GetEntityInstance(sceneID, entityID);
	if (entityInstance.ScriptClass->OnCollision3DEndMethod)
	{
		float value = 5.0f;
		void* args[] = {&value};
		MonoApi::CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnCollision3DEndMethod, args);
	}
}

void ScriptEngine::OnScriptComponentDestroyed(UUID sceneID, UUID entityID)
{
	Debug::Assert(Instance()._entityInstanceMap.find(sceneID) != Instance()._entityInstanceMap.end());
	auto& entityMap = Instance()._entityInstanceMap.at(sceneID);
	Debug::Assert(entityMap.find(entityID) != entityMap.end());
	entityMap.erase(entityID);
}

bool ScriptEngine::ModuleExists(const String& moduleName)
{
	MonoClass* monoClass = MonoApi::GetClass(Instance()._appImage, moduleName);
	return monoClass != nullptr;
}

void ScriptEngine::CreateScriptEntity(Entity entity)
{
	Scene* scene = entity.GetScene();
	const UUID id = entity.GetComponent<IDComponent>().ID;
	auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;
	if (moduleName.empty()) return;

	if (!ModuleExists(moduleName))
	{
		Log::CoreError("Entity references non-existent script module '{0}'", moduleName);
		return;
	}

	EntityScriptClass& scriptClass = Instance()._entityClassMap[moduleName];
	scriptClass.SetScript(moduleName);

	scriptClass.Class = Instance().GetClass(Instance()._appImage, scriptClass);
	scriptClass.SyncClassMethods(Instance()._appImage);

	EntityInstance& entityInstance = Instance()._entityInstanceMap[scene->GetUUID()][id];
	entityInstance.ScriptClass = &scriptClass;
	ScriptModuleFieldMap& moduleFieldMap = entityInstance.ModuleFieldMap;
	auto& fieldMap = moduleFieldMap[moduleName];

	// Save old fields
	HashMap<String, PublicField> oldFields;
	oldFields.reserve(fieldMap.size());
	for (auto& [fieldName, field] : fieldMap) oldFields.emplace(fieldName, Move(field));
	fieldMap.clear();

	// Retrieve public fields (TODO: cache these fields if the module is used more than once)
	{
		MonoClassField* iter;
		void* ptr = nullptr;
		while ((iter = MonoApi::GetFields(scriptClass.Class, &ptr)) != nullptr)
		{
			const auto name = MonoApi::GetFieldName(iter);
			const uint flags = MonoApi::GetFieldFlags(iter);
			if (!MonoApi::IsPublic(flags)) continue;

			auto* monoFieldType = MonoApi::GetFieldType(iter);
			const auto fieldType = MonoApi::GetSaffronPublicFieldType(monoFieldType);

			// TODO: Attributes
			//auto* attr = MonoApi::GetCustomAttrInfo(scriptClass.Class, iter);

			if (oldFields.find(name) != oldFields.end())
			{
				fieldMap.emplace(name, Move(oldFields.at(name)));
			}
			else
			{
				PublicField field(name, fieldType);
				field._entityInstance = &entityInstance;
				field._monoClassField = iter;
				fieldMap.emplace(name, Move(field));
			}
		}
	}
}

void ScriptEngine::DeleteScriptEntity(Entity entity, const String& moduleName)
{
	EntityInstance& entityInstance = GetEntityInstance(entity.GetSceneUUID(), entity.GetUUID());
	ScriptModuleFieldMap& moduleFieldMap = entityInstance.ModuleFieldMap;
	if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
	{
		moduleFieldMap.erase(moduleName);
	}
}

void ScriptEngine::InstantiateScriptEntity(Entity entity)
{
	Scene* scene = entity.GetScene();
	UUID id = entity.GetComponent<IDComponent>().ID;
	auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;

	EntityInstance& entityInstance = GetEntityInstance(scene->GetUUID(), id);
	Debug::Assert(entityInstance.ScriptClass != nullptr);
	entityInstance.Handle = MonoApi::Instantiate(Instance()._monoDomain, *entityInstance.ScriptClass);

	MonoClass* entityClass = Instance().GetClass(Instance().GetCoreImage(), "Se", "Entity");
	Debug::Assert(entityClass, "Failed to load entity class when checking for subclass");

	if (MonoApi::IsSubclass(entityInstance.ScriptClass->Class, entityClass, false))
	{
		auto* entityIDProperty = MonoApi::GetProperty(entityInstance.ScriptClass->Class, "ID");
		auto* entityIDSetMethod = MonoApi::GetSetter(entityIDProperty);
		void* param[] = {&id};
		MonoApi::CallMethod(entityInstance.GetInstance(), entityIDSetMethod, param);
	}

	// Set all public fields to appropriate values
	ScriptModuleFieldMap& moduleFieldMap = entityInstance.ModuleFieldMap;
	if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
	{
		auto& publicFields = moduleFieldMap.at(moduleName);
		for (auto& [name, field] : publicFields)
		{
			field.CopyStoredValueToRuntime();
		}
	}

	// Call OnCreate function (if exists)
	Instance().OnCreateEntity(entity);
}

EntityInstance& ScriptEngine::GetEntityInstance(UUID sceneID, UUID entityID)
{
	Debug::Assert(GetEntityInstanceMap().find(sceneID) != GetEntityInstanceMap().end(), "Invalid scene ID!");
	auto& entityIDMap = GetEntityInstanceMap().at(sceneID);
	Debug::Assert(entityIDMap.find(entityID) != entityIDMap.end(), "Invalid entity ID!");
	return entityIDMap.at(entityID);
}

MonoString* ScriptEngine::CreateMonoString(const String& string)
{
	return MonoApi::CreateString(Instance()._monoDomain, string);
}

void ScriptEngine::AttachThread()
{
	MonoApi::AttachThread(_monoDomain);
}

void ScriptEngine::DetachThread()
{
	MonoApi::DetachThread();
}

MonoImage* ScriptEngine::GetCoreImage() const
{
	return _coreImage;
}

MonoImage* ScriptEngine::GetAppImage() const
{
	return _appImage;
}

MonoAssembly* ScriptEngine::GetCoreAssembly() const
{
	return _coreAssembly;
}

MonoAssembly* ScriptEngine::GetAppAssembly() const
{
	return _appAssembly;
}

MonoClass* ScriptEngine::GetClass(MonoImage* image, const EntityScriptClass& scriptClass)
{
	if (_classCacheMap.find(scriptClass.FullName) == _classCacheMap.end())
	{
		auto* monoClass = MonoApi::GetClass(image, scriptClass);
		if (!monoClass) Log::CoreWarn("mono_class_from_name failed");
		_classCacheMap.emplace(scriptClass.FullName, monoClass);
	}
	return _classCacheMap[scriptClass.FullName];
}

MonoClass* ScriptEngine::GetClass(MonoImage* image, const String& namespaceName, const String& className)
{
	const String fullName = namespaceName.empty() ? className : namespaceName + "." + className;
	if (_classCacheMap.find(fullName) == _classCacheMap.end())
	{
		auto* monoClass = MonoApi::GetClass(image, namespaceName, className);
		if (!monoClass) Log::CoreWarn("mono_class_from_name failed");
		_classCacheMap.emplace(fullName, monoClass);
	}
	return _classCacheMap[fullName];
}

EntityInstanceMap& ScriptEngine::GetEntityInstanceMap()
{
	return Instance()._entityInstanceMap;
}
}

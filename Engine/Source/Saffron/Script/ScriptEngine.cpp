#include "SaffronPCH.h"

#include <monopp/MonoCpp.h>

#include "Saffron/Entity/Entity.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/Script/ScriptRegistry.h"
#include "Saffron/Core/GenUtils.h"

namespace Se
{
ScriptEngine::ScriptEngine() :
	SingleTon(this),
	_scriptEngineRegistry(CreateUnique<ScriptRegistry>())
{
#ifdef SE_DEBUG
	Mono::Initialize(MonoPath, ScriptsPath, "", "Saffron", true);
#else
	Mono::Initialize(ScriptsPath, "", "Saffron", false)
#endif

	_monoDomain = CreateUnique<Mono::Domain>(FriendlyDomainName);
	Mono::Domain::SetCurrentDomain(*_monoDomain);

	_coreAssembly = Mono::Assembly(*_monoDomain, ScriptCoreScript);

	const auto& entityType = GetOrLoadCoreModule("Saffron", "Entity");
	ScriptEntity::OnCreateMethod = entityType.GetMethod("OnCreate()");
	ScriptEntity::OnUpdateMethod = entityType.GetMethod("OnUpdate(single)");
	ScriptEntity::IdProperty = entityType.GetProperty("ID");
	ScriptEntity::OnCollision2DBeginMethod = entityType.GetMethod("OnCollision2DBegin(single)");
	ScriptEntity::OnCollision2DEndMethod = entityType.GetMethod("OnCollision2DEnd(single)");
}

ScriptEngine::~ScriptEngine()
{
	_sceneContext = nullptr;
	_scriptEntities.clear();
}

void ScriptEngine::OnUpdate()
{
}

void ScriptEngine::OnGuiRender()
{
	ImGui::Begin("Script Engine Debug");
	for (auto& [sceneID, entityMap] : Instance()._scriptEntities)
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
				/*if (opened)
				{
					for (auto& [name, field] : entityInstance.Fields)
					{
						opened = ImGui::TreeNode(name.c_str());
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
				}*/
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
			if (Instance()._scriptEntities.find(uuid) != Instance()._scriptEntities.end())
			{
				Instance()._modules.clear();
				Instance()._scriptEntities.at(uuid).clear();
				Instance()._scriptEntities.erase(uuid);
			}
		}
		Instance()._modules.clear();
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

void ScriptEngine::LoadRuntimeAssembly(const Path& assemblyFilepath)
{
	auto& domain = *Instance()._monoDomain;
	auto& coreAssembly = Instance()._coreAssembly;
	auto& appAssembly = Instance()._appAssembly;

	coreAssembly = domain.GetAssembly(ScriptCoreScript, true);
	appAssembly = domain.GetAssembly(assemblyFilepath.string(), true);

	ScriptRegistry::Instance().RegisterAll();
}

void ScriptEngine::ReloadAssembly(const Path& assemblyFilepath)
{
	Instance()._assemblyFilepath = ScriptsPath + assemblyFilepath.string() + ScriptsExtension;
	LoadRuntimeAssembly(Instance()._assemblyFilepath);
	Instance()._modules.clear();
	if (!Instance()._scriptEntities.empty())
	{
		auto scene = GetSceneContext();
		Debug::Assert(scene, "No active scene!");
		if (Instance()._scriptEntities.find(scene->GetUUID()) != Instance()._scriptEntities.end())
		{
			auto& entityMap = Instance()._scriptEntities.at(scene->GetUUID());
			for (auto& [entityID, entityInstance] : entityMap)
			{
				const auto& entityMap = scene->GetEntityMap();
				Debug::Assert(entityMap.find(entityID) != entityMap.end(),
				              "Invalid entity ID or entity doesn't exist in scene!");
				Instance().CreateScriptEntity(entityMap.at(entityID));
			}
		}
	}
}

void ScriptEngine::SetSceneContext(const Shared<Scene>& scene)
{
	Instance()._sceneContext = scene;
}

auto ScriptEngine::GetSceneContext() -> const Shared<Scene>&
{
	return Instance()._sceneContext;
}

void ScriptEngine::CopyEntityScriptData(UUID dst, UUID src)
{
	auto& sceneMap = Instance().GetEntityInstanceMap();

	Debug::Assert(sceneMap.find(dst) != sceneMap.end());
	Debug::Assert(sceneMap.find(src) != sceneMap.end());

	auto& dstEntityMap = sceneMap.at(dst);
	auto& srcEntityMap = sceneMap.at(src);

	//for (auto& [entityID, entityInstance] : srcEntityMap)
	//{
	//	for (auto& [moduleName, srcFieldMap] : srcEntityMap[entityID].ModuleFieldMap)
	//	{
	//		auto& dstModuleFieldMap = dstEntityMap[entityID].ModuleFieldMap;
	//		for (auto& [fieldName, field] : srcFieldMap)
	//		{
	//			Debug::Assert(dstModuleFieldMap.find(moduleName) != dstModuleFieldMap.end());
	//			auto& fieldMap = dstModuleFieldMap.at(moduleName);
	//			Debug::Assert(fieldMap.find(fieldName) != fieldMap.end());
	//			fieldMap.at(fieldName).SetStoredValue(static_cast<void*>(field._storedValueBuffer));
	//		}
	//	}
	//}
}

void ScriptEngine::OnCreateEntity(Entity entity)
{
	OnCreateEntity(entity.GetSceneUUID(), entity.GetComponent<IDComponent>().ID);
}

void ScriptEngine::OnCreateEntity(UUID sceneID, UUID entityID)
{
	auto& entity = GetScriptEntity(sceneID, entityID);

	GetInvoker<void()>(ScriptEntity::OnCreateMethod, entity.GetMonoObject())();
}

void ScriptEngine::OnUpdateEntity(UUID sceneID, UUID entityID, Time ts)
{
	auto& entity = GetScriptEntity(sceneID, entityID);
	GetInvoker<void(float)>(ScriptEntity::OnCollision2DBeginMethod, entity.GetMonoObject())(ts.sec());
}

void ScriptEngine::OnCollision2DBegin(Entity entity)
{
	OnCollision2DBegin(entity.GetSceneUUID(), entity.GetUUID());
}

void ScriptEngine::OnCollision2DBegin(UUID sceneID, UUID entityID)
{
	auto& entity = GetScriptEntity(sceneID, entityID);
	float value = 5.0f;
	GetInvoker<void(float)>(ScriptEntity::OnCollision2DBeginMethod, entity.GetMonoObject())(value);
}

void ScriptEngine::OnCollision2DEnd(Entity entity)
{
	OnCollision2DEnd(entity.GetSceneUUID(), entity.GetUUID());
}

void ScriptEngine::OnCollision2DEnd(UUID sceneID, UUID entityID)
{
	auto& entity = GetScriptEntity(sceneID, entityID);
	float value = 5.0f;
	GetInvoker<void(float)>(ScriptEntity::OnCollision2DEndMethod, entity.GetMonoObject())(value);
}

void ScriptEngine::OnCollision3DBegin(Entity entity)
{
	OnCollision3DBegin(entity.GetSceneUUID(), entity.GetUUID());
}

void ScriptEngine::OnCollision3DBegin(UUID sceneID, UUID entityID)
{
	auto& entity = GetScriptEntity(sceneID, entityID);
	float value = 5.0f;
	GetInvoker<void(float)>(ScriptEntity::OnCollision3DBeginMethod, entity.GetMonoObject())(value);
}

void ScriptEngine::OnCollision3DEnd(Entity entity)
{
	OnCollision3DEnd(entity.GetSceneUUID(), entity.GetUUID());
}

void ScriptEngine::OnCollision3DEnd(UUID sceneID, UUID entityID)
{
	auto& entity = GetScriptEntity(sceneID, entityID);
	float value = 5.0f;
	GetInvoker<void(float)>(ScriptEntity::OnCollision3DEndMethod, entity.GetMonoObject())(value);
}

void ScriptEngine::OnScriptComponentDestroyed(UUID sceneID, UUID entityID)
{
	Debug::Assert(Instance()._scriptEntities.find(sceneID) != Instance()._scriptEntities.end());
	auto& entityMap = Instance()._scriptEntities.at(sceneID);
	Debug::Assert(entityMap.find(entityID) != entityMap.end());
	entityMap.erase(entityID);
}

auto ScriptEngine::ModuleExists(const String& fullname) -> bool
{
	auto strings = GenUtils::SplitString(fullname, ".");
	if (strings.size() == 1)
	{
		return ModuleExists("", strings[0]);
	}
	if (strings.size() == 2)
	{
		return ModuleExists(strings[0], strings[1]);
	}
	Debug::Break("Empty module");
	return false;
}

auto ScriptEngine::ModuleExists(const String& namespaceName, const String& name) -> bool
{
	return Instance().GetAppAssembly().TypeExists(namespaceName, name);
}

void ScriptEngine::CreateScriptEntity(Entity entity)
{
	auto& sc = entity.GetComponent<ScriptComponent>();
	if (sc.Name.empty() || sc.Fullname.empty())
	{
		Log::CoreWarn("Script component was found empty when constructing a script entity");
		return;
	}

	const auto& module = GetOrLoadModule(sc.NamespaceName, sc.Name);

	auto& scene = *entity.GetScene();

	auto& sceneContainer = _scriptEntities[scene.GetUUID()];
	auto [pair, didEmplace] = sceneContainer.emplace(entity.GetUUID(), Move(ScriptEntity(module, scene)));
	auto& scriptEntity = pair->second;

	const auto& fullname = module.GetFullname();
	const auto it = _moduleFieldMaps.find(fullname);
	if (it == _moduleFieldMaps.end())
	{
		const auto& fields = module.GetFields();
		for (const auto& monoField : fields)
		{
			if (monoField.GetVisibility() == Mono::Visibility::Public)
			{
				_moduleFieldMaps[fullname].emplace_back(scriptEntity, monoField);
			}
		}
	}
}

void ScriptEngine::DeleteScriptEntity(Entity entity)
{
	auto& scene = *entity.GetScene();
	Instance()._scriptEntities.at(scene.GetUUID()).erase(entity.GetUUID());
}

void ScriptEngine::InstantiateScriptEntity(Entity entity)
{
	auto* scene = entity.GetScene();
	auto id = entity.GetUUID();

	Debug::Assert(entity.HasComponent<ScriptComponent>(),
	              "Tried to instansiate entity without a script component. Add a script component to entity ({0}:{1}) to resolve this.",
	              id, entity.GetComponent<TagComponent>().Tag);

	auto& moduleName = entity.GetComponent<ScriptComponent>().Fullname;

	auto& scriptEntity = Instance().GetScriptEntity(scene->GetUUID(), id);
	Debug::Assert(scriptEntity.Valid(), "Entity was not valid: ({0}:{1})", id, entity.GetComponent<TagComponent>().Tag);
	scriptEntity.CreateNewObject();

	const auto idInvoker = Mono::CreatePropertyInvoker<ulong>(ScriptEntity::IdProperty);
	idInvoker.SetValue(scriptEntity.GetMonoObject(), entity.GetUUID());

	auto& publicFields = Instance()._moduleFieldMaps.at(moduleName);
	for (auto& field : publicFields)
	{
		field.CopyStoredValueToRuntime();
	}

	// Call OnCreate function (if exists)
	Instance().OnCreateEntity(entity);
}

auto ScriptEngine::GetScriptEntity(UUID sceneID, UUID entityID) -> ScriptEntity&
{
	Debug::Assert(GetEntityInstanceMap().find(sceneID) != GetEntityInstanceMap().end(), "Invalid scene ID!");
	auto& entities = GetEntityInstanceMap().at(sceneID);

	Debug::Assert(entities.find(entityID) != entities.end(), "Invalid entity ID!");
	return entities.at(entityID);
}

auto ScriptEngine::GetScriptEntity(UUID sceneID, UUID entityID) const -> const ScriptEntity&
{
	return const_cast<ScriptEngine&>(*this).GetScriptEntity(sceneID, entityID);
}

auto ScriptEngine::GetFieldMap(const String& namespaceName, const String& name) -> List<Field>&
{
	Debug::Assert(ModuleExists(namespaceName, name), "Module did not exist: {0}.{1}", namespaceName, name);
	return _moduleFieldMaps.at(namespaceName + "." + name);
}

auto ScriptEngine::GetFieldMap(const String& namespaceName, const String& name) const -> const List<Field>&
{
	return const_cast<ScriptEngine&>(*this).GetFieldMap(namespaceName, name);
}

void ScriptEngine::AttachThread()
{
	_monoDomain->AttachThread();
}

void ScriptEngine::DetachThread()
{
	_monoDomain->DetachThread();
}

auto ScriptEngine::GetCoreAssembly() -> const Mono::Assembly&
{
	return Instance()._coreAssembly;
}

auto ScriptEngine::GetAppAssembly() -> const Mono::Assembly&
{
	return Instance()._appAssembly;
}

auto ScriptEngine::GetEntityType() -> const Mono::Type&
{
	return Instance()._coreModules.at("Saffron.Entity");
}

auto ScriptEngine::GetOrLoadModule(const String& namespaceName, const String& name) const -> const Mono::Type&
{
	const auto concat = namespaceName + "." + name;
	const auto it = _modules.find(concat);
	if (it == _modules.end())
	{
		const auto [pair, didEmplace] = _modules.emplace(concat, _appAssembly.GetType(namespaceName, name));
		return pair->second;
	}
	return it->second;
}

auto ScriptEngine::GetOrLoadCoreModule(const String& namespaceName, const String& name) const -> const Mono::Type&
{
	const auto concat = namespaceName + "." + name;
	const auto it = _coreModules.find(concat);
	if (it == _coreModules.end())
	{
		const auto [pair, didEmplace] = _coreModules.emplace(concat, _coreAssembly.GetType(namespaceName, name));
		auto fields = pair->second.GetFields();
		return pair->second;
	}
	return it->second;
}

auto ScriptEngine::GetEntityInstanceMap() -> HashMap<UUID, HashMap<UUID, ScriptEntity>>&
{
	return Instance()._scriptEntities;
}
}

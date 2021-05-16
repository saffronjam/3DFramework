#pragma once

#include <monopp/Type.h>
#include <monopp/MethodInvoker.h>

#include "Saffron/Base.h"
#include "Saffron/Core/Project.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Script/ScriptEntity.h"

namespace Mono
{
class Domain;
class Assembly;
}

namespace Se
{
class ScriptRegistry;

class ScriptEngine : public SingleTon<ScriptEngine>
{
private:

	// Intentionally undefined
	template <class Signature>
	class MethodInvoker;

	template <typename... Args>
	class MethodInvoker<void(Args ...)>
	{
		using Signature = void(Args ...);
		inline static const String ExceptionBase = Log::Fmt::OnYellow + Log::Fmt::Black + "Mono Warning" +
			Log::Fmt::Warn;

	public:
		void operator()(Args ... args);

		void Invoke(Args ... args);

		static auto Create(const Mono::Method& method, const Mono::Object& object) -> MethodInvoker;

	private:
		explicit MethodInvoker(Mono::MethodInvoker<Signature> monoInvoker, Mono::Object monoObject);

	private:
		Mono::MethodInvoker<void(Args ...)> _monoInvoker;
		Mono::Object _monoObject;
	};

	template <typename ReturnType, typename... Args>
	class MethodInvoker<ReturnType(Args ...)>
	{
		using Signature = ReturnType(Args ...);
		inline static const String ExceptionBase = Log::Fmt::OnYellow + Log::Fmt::Black + "Mono Warning" +
			Log::Fmt::Warn;

	public:
		auto operator()(Args ... args) -> ReturnType;

		auto Invoke(Args ... args) -> ReturnType;

		static auto Create(const Mono::Method& method, const Mono::Object& object) -> MethodInvoker;

	private:
		explicit MethodInvoker(Mono::MethodInvoker<Signature> monoInvoker, Mono::Object monoObject);

	private:
		Mono::MethodInvoker<void(Args ...)> _monoInvoker;
		Mono::Object _monoObject;
	};

public:
	ScriptEngine();
	~ScriptEngine();

	static void OnUpdate();
	static void OnGuiRender();
	static void OnProjectChange(const Shared<Project>& project);
	static void OnSceneChange(const Shared<Scene>& scene);

	static void LoadRuntimeAssembly(const Path& assemblyFilepath);
	static void ReloadAssembly(const Path& assemblyFilepath);

	static void SetSceneContext(const Shared<Scene>& scene);
	static auto GetSceneContext() -> const Shared<Scene>&;

	static void CopyEntityScriptData(UUID dst, UUID src);

	void OnCreateEntity(Entity entity);
	void OnCreateEntity(UUID sceneID, UUID entityID);
	void OnUpdateEntity(UUID sceneID, UUID entityID, Time ts);

	void OnCollision2DBegin(Entity entity);
	void OnCollision2DBegin(UUID sceneID, UUID entityID);
	void OnCollision2DEnd(Entity entity);
	void OnCollision2DEnd(UUID sceneID, UUID entityID);

	void OnCollision3DBegin(Entity entity);
	void OnCollision3DBegin(UUID sceneID, UUID entityID);
	void OnCollision3DEnd(Entity entity);
	void OnCollision3DEnd(UUID sceneID, UUID entityID);

	static void OnScriptComponentDestroyed(UUID sceneID, UUID entityID);

	static auto ModuleExists(const String& fullname) -> bool;
	static auto ModuleExists(const String& namespaceName, const String& name) -> bool;

	void CreateScriptEntity(Entity entity);
	void DeleteScriptEntity(Entity entity);
	void InstantiateScriptEntity(Entity entity);

	auto GetEntityInstanceMap() -> HashMap<UUID, HashMap<UUID, ScriptEntity>>&;
	auto GetScriptEntity(UUID sceneID, UUID entityID) -> ScriptEntity&;
	auto GetScriptEntity(UUID sceneID, UUID entityID) const -> const ScriptEntity&;
	auto GetFieldMap(const String& namespaceName, const String& name) -> List<Field>&;
	auto GetFieldMap(const String& namespaceName, const String& name) const -> const List<Field>&;

	void AttachThread();
	void DetachThread();

	auto GetCoreAssembly() -> const Mono::Assembly&;
	auto GetAppAssembly() -> const Mono::Assembly&;

	static auto GetEntityType() -> const Mono::Type&;

public:
	static constexpr const char* MonoPath = "../Engine/ThirdParty/mono/bin/Runtime";

private:
	auto GetOrLoadModule(const String& namespaceName, const String& name) const -> const Mono::Type&;
	auto GetOrLoadCoreModule(const String& namespaceName, const String& name) const -> const Mono::Type&;

	template <typename Signature>
	auto GetInvoker(const Mono::Method& method, const Mono::Object& object) -> MethodInvoker<Signature>
	{
		return MethodInvoker<Signature>::Create(method, object);
	}

private:
	Unique<ScriptRegistry> _scriptEngineRegistry;

	Unique<Mono::Domain> _monoDomain;
	Path _assemblyFilepath;
	Shared<Project> _projectContext;
	Shared<Scene> _sceneContext;

	Mono::Assembly _coreAssembly;
	Mono::Assembly _appAssembly;

	HashMap<UUID, HashMap<UUID, ScriptEntity>> _scriptEntities;
	mutable HashMap<String, Mono::Type> _modules;
	mutable HashMap<String, Mono::Type> _coreModules;
	// ModuleName: <Fieldname: Field>
	mutable HashMap<String, List<Field>> _moduleFieldMaps;
	mutable HashMap<String, List<Field>> _coreModuleFieldMaps;

	static constexpr const char* ScriptsPath = "Assets/Scripts/";
	static constexpr const char* ScriptsExtension = ".dll";

	static constexpr const char* FriendlyDomainName = "Saffron Runtime";
	static constexpr const char* ScriptCoreScript = "Assets/Scripts/ScriptCore.dll";
};

template <typename ... Args>
void ScriptEngine::MethodInvoker<void(Args ...)>::operator()(Args ... args)
{
	Invoke(args...);
}

template <typename ... Args>
void ScriptEngine::MethodInvoker<void(Args ...)>::Invoke(Args ... args)
{
	try
	{
		_monoInvoker(_monoObject, args...);
	}
	catch (Mono::MonoException& exception)
	{
		Log::CoreWarn(ExceptionBase + " Failed to invoke Mono method {0}, Reason: \n{1}",
		              _monoInvoker.GetFullDeclname(), exception.what());
	}
}

template <typename ... Args>
auto ScriptEngine::MethodInvoker<void(Args ...)>::Create(const Mono::Method& method,
                                                         const Mono::Object& object) -> MethodInvoker
{
	return MethodInvoker(Mono::CreateMethodInvoker<Signature>(method), object);
}

template <typename ... Args>
ScriptEngine::MethodInvoker<void(Args ...)>::MethodInvoker(Mono::MethodInvoker<Signature> monoInvoker,
                                                           Mono::Object monoObject):
	_monoInvoker(Move(monoInvoker)),
	_monoObject(Move(monoObject))
{
}

template <typename ReturnType, typename ... Args>
auto ScriptEngine::MethodInvoker<ReturnType(Args ...)>::operator()(Args ... args) -> ReturnType
{
	return Invoke(args...);
}

template <typename ReturnType, typename ... Args>
auto ScriptEngine::MethodInvoker<ReturnType(Args ...)>::Invoke(Args ... args) -> ReturnType
{
	try
	{
		return _monoInvoker(_monoObject, args...);
	}
	catch (Mono::MonoException& exception)
	{
		Log::CoreWarn(ExceptionBase + " Failed to invoke Mono method {0}, Reason: \n{1}",
		              _monoInvoker.GetFullDeclname(), exception.what());
	}
	return ReturnType();
}

template <typename ReturnType, typename ... Args>
auto ScriptEngine::MethodInvoker<ReturnType(Args ...)>::Create(const Mono::Method& method,
                                                               const Mono::Object& object) -> MethodInvoker
{
	return MethodInvoker(Mono::CreateMethodInvoker<Signature>(method), object);
}

template <typename ReturnType, typename ... Args>
ScriptEngine::MethodInvoker<ReturnType(Args ...)>::MethodInvoker(Mono::MethodInvoker<Signature> monoInvoker,
                                                                 Mono::Object monoObject):
	_monoInvoker(Move(monoInvoker)),
	_monoObject(Move(monoObject))
{
}
}

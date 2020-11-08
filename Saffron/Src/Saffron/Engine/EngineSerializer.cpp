#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Engine/Engine.h"
#include "Saffron/Engine/EngineSerializer.h"

namespace Se
{
void EngineSerializer::Serialize(const Filepath &filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "StartUpSceneFilepath";
	out << YAML::Value << Engine::m_StartUpSceneFilepath.string();
	out << YAML::EndMap;

	OutputStream fout(filepath);
	fout << out.c_str();
}

bool EngineSerializer::Deserialize(const Filepath &filepath)
{
	InputStream stream(filepath);
	StringStream strStream;
	strStream << stream.rdbuf();

	SE_CORE_INFO("Deserializing Engine Properties");
	YAML::Node data = YAML::Load(strStream.str());

	auto startUpSceneFilepath = data["StartUpSceneFilepath"];
	if ( !startUpSceneFilepath )
	{
		SE_CORE_WARN("No scene filepath found in Engine Properties");
	}
	else
	{
		Engine::m_StartUpSceneFilepath = startUpSceneFilepath.as<String>();
	}
	return true;
}
}

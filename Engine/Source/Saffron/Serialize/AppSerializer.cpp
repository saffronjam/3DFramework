#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Serialize/AppSerializer.h"
#include "Saffron/Serialize/ProjectSerializer.h"
#include "Saffron/Scene/EditorScene.h"

namespace Se
{
AppSerializer::AppSerializer(App& application) :
	_application(application)
{
}

void AppSerializer::Serialize(const Filepath& filepath) const
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "App";
	out << YAML::Value << App::GetPlatformName() + String(" ") + App::GetConfigurationName();
	out << YAML::Key << "Recent Projects";
	out << YAML::Value << YAML::BeginSeq;

	for (const auto& project : _application.GetRecentProjectList())
	{
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectFilepath" << YAML::Value << project->GetProjectFilepath().string();
		out << YAML::EndMap;
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	OutputStream fout(filepath);
	fout << out.c_str();
}

bool AppSerializer::Deserialize(const Filepath& filepath)
{
	InputStream stream(filepath);
	StringStream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["App"]) return false;

	
	Log::CoreInfo("Deserializing App");

	// Deserializing recent projects
	auto recentProjects = data["Recent Projects"];
	if (recentProjects)
	{
		for (const auto& recentProjectRef : recentProjects)
		{
			auto recentProjectFilepathNode = recentProjectRef["ProjectFilepath"];
			if (!recentProjectFilepathNode)
			{
				continue;
			}

			Filepath projectFilepath = recentProjectFilepathNode.as<String>();
			if (!FileIOManager::FileExists(projectFilepath))
			{
				continue;
			}

			auto project = Shared<Project>::Create(projectFilepath);
			if (project->IsValid())
			{
				_application.AddProject(project);
			}
		}
	}

	return true;
}
}

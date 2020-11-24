#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Serialize/ApplicationSerializer.h"
#include "Saffron/Serialize/ProjectSerializer.h"
#include "Saffron/Scene/EditorScene.h"

namespace Se
{

ApplicationSerializer::ApplicationSerializer(Application &application)
	: m_Application(application)
{
}

void ApplicationSerializer::Serialize(const Filepath &filepath) const
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Application";
	out << YAML::Value << Application::GetPlatformName() + String(" ") + Application::GetConfigurationName();
	out << YAML::Key << "Recent Projects";
	out << YAML::Value << YAML::BeginSeq;

	for ( const auto &project : m_Application.GetRecentProjectList() )
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

bool ApplicationSerializer::Deserialize(const Filepath &filepath)
{
	InputStream stream(filepath);
	StringStream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if ( !data["Application"] )
		return false;

	SE_CORE_INFO("Deserializing Application");

	// Deserializing recent projects
	auto recentProjects = data["Recent Projects"];
	if ( recentProjects )
	{
		for ( const auto &recentProjectRef : recentProjects )
		{
			auto recentProjectFilepathNode = recentProjectRef["ProjectFilepath"];
			if ( !recentProjectFilepathNode )
			{
				continue;
			}

			Filepath projectFilepath = recentProjectFilepathNode.as<String>();
			if ( !FileIOManager::FileExists(projectFilepath) )
			{
				continue;
			}

			auto project = Shared<Project>::Create(projectFilepath);
			if ( project->IsValid() )
			{
				m_Application.AddProject(project);
			}
		}
	}

	return true;
}
}

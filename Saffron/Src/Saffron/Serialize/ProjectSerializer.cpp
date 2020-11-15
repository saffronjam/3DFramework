#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Core/DateTime.h"
#include "Saffron/Serialize/ProjectSerializer.h"
#include "Saffron/Scene/EditorScene.h"

namespace YAML {

template<>
struct convert<Se::DateTime>
{
	static Node encode(const Se::DateTime &rhs)
	{
		Node node;
		node.push_back(rhs.Year());
		node.push_back(rhs.Month());
		node.push_back(rhs.Day());
		node.push_back(rhs.Weekday());
		node.push_back(rhs.Hour());
		node.push_back(rhs.Minutes());
		node.push_back(rhs.Seconds());
		return node;
	}

	static bool decode(const Node &node, Se::DateTime &rhs)
	{
		if ( !node.IsSequence() || node.size() != 7 )
			return false;

		rhs = Se::DateTime(node[0].as<int>(),
						   node[1].as<int>(),
						   node[2].as<int>(),
						   node[3].as<int>(),
						   node[4].as<int>(),
						   node[5].as<int>(),
						   node[6].as<int>());
		return true;
	}
};

}

namespace Se
{

YAML::Emitter &operator<<(YAML::Emitter &out, const DateTime &dateTime)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << dateTime.Year() << dateTime.Month() << dateTime.Day() << dateTime.Weekday() << dateTime.Hour() << dateTime.Minutes() << dateTime.Seconds() << YAML::EndSeq;
	return out;
}

ProjectSerializer::ProjectSerializer(Project &project)
	: m_Project(project)
{
}

void ProjectSerializer::Serialize(const Filepath &filepath) const
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Project" << m_Project.GetName();
	out << YAML::Key << "UUID" << YAML::Value << static_cast<Uint64>(m_Project.GetUUID());
	out << YAML::Key << "LastOpened" << YAML::Value << m_Project.LastOpened();

	out << YAML::Key << "ProjectFilepath" << YAML::Value << m_Project.GetProjectFilepath().string();
	out << YAML::Key << "Scenes";
	out << YAML::Value << YAML::BeginSeq;
	for ( const auto &scene : m_Project.GetSceneFilepaths() )
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << scene.string();
		out << YAML::Key << "SceneFilepath" << YAML::Value << scene.string();
		out << YAML::EndMap;
	}
	out << YAML::Value << YAML::EndSeq;

	out << YAML::EndMap;

	OutputStream fout(filepath);
	fout << out.c_str();
}

bool ProjectSerializer::Deserialize(const Filepath &filepath)
{
	InputStream stream(filepath);
	StringStream strStream;
	strStream << stream.rdbuf();

	YAML::Node project = YAML::Load(strStream.str());
	if ( !project["Project"] )
		return false;

	SE_CORE_INFO("Deserializing Project {}", project["Project"].as<String>());

	std::array<YAML::Node, 5> nodes{ project["Project"], project["UUID"],project["ProjectFilepath"], project["Scenes"], project["LastOpened"] };
	bool badNode = false;
	for ( auto &node : nodes )
	{
		if ( !node )
		{
			badNode = true;
			break;
		}
	}
	if ( badNode )
	{
		SE_CORE_WARN("Bad project data in application file");
		return false;
	}

	ArrayList<Filepath> sceneFilepaths;
	for ( const auto &sceneRef : project["Scenes"] )
	{
		const auto &sceneFilepathNode = sceneRef["SceneFilepath"];
		if ( sceneFilepathNode )
		{
			sceneFilepaths.push_back(sceneFilepathNode.as<String>());
		}
	}
	m_Project.m_UUID = project["UUID"].as<Uint64>();
	m_Project.m_Name = project["Project"].as<String>();
	m_Project.m_LastOpened = project["LastOpened"].as<DateTime>();
	m_Project.m_ProjectFilepath = project["ProjectFilepath"].as<String>();
	m_Project.m_SceneFilepaths = Move(sceneFilepaths);
	m_Project.m_SceneCache.clear();

	return true;
}

}

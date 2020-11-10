#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Serialize//ApplicationSerializer.h"



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
	out << YAML::Key << "Projects";
	out << YAML::Value << YAML::BeginSeq;

	for ( const auto &project : m_Application.GetProjectList() )
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << project.Name;
		out << YAML::Key << "UUID" << YAML::Value << static_cast<Uint64>(project.UUID);
		out << YAML::Key << "SceneFilepath" << YAML::Value << project.SceneFilepath.string();
		out << YAML::Key << "TexturePreviewFilepath" << YAML::Value << project.PreviewTexture->GetFilepath().string();
		out << YAML::Key << "LastOpened" << YAML::Value << project.LastOpened;
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

	auto projects = data["Projects"];
	if ( projects )
	{
		for ( const auto &project : projects )
		{
			std::array<YAML::Node, 5> nodes{ project["Project"], project["UUID"], project["SceneFilepath"],project["TexturePreviewFilepath"], project["LastOpened"] };
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
				continue;
			}
			Shared<Texture2D> previewTexture = Texture2D::Create(nodes[3].as<String>());
			m_Application.m_ProjectList.push_back(Application::Project{ nodes[1].as<Uint64>(),nodes[0].as<String>(), nodes[2].as<String>(), previewTexture, nodes[4].as<DateTime>() });
		}
	}

	return true;
}
}

#include "SaffronPCH.h"

#include "Saffron/Gui/Gui.h"
#include "Saffron/Script/ScriptManager.h"

namespace Se
{

std::filesystem::path ScriptManager::m_ScriptFolderPath;
std::vector<std::filesystem::directory_entry> ScriptManager::m_ScriptPaths;
std::vector<ScriptManager::ScriptName> ScriptManager::m_ScriptNames;
std::map<std::string, Ref<Texture2D>> ScriptManager::m_TexStore;

void ScriptManager::Init(std::filesystem::path path)
{
	m_ScriptFolderPath = std::move(path);

	m_TexStore["ScriptCS"] = Texture2D::Create("Assets/Editor/ScriptCS.png");

	SyncScriptPaths();
}

void ScriptManager::OnGuiRender()
{

	ImGui::Begin("Scripts");

	ImGui::ShowDemoWindow();

	const int noCollums = std::max(1, static_cast<int>(ImGui::GetContentRegionAvailWidth() / 100.0f));

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * static_cast<float>(noCollums));
	ImGui::Columns(noCollums, nullptr, false);

	for ( size_t i = 0; i < m_ScriptNames.size(); i++ )
	{
		ImGui::Button(m_ScriptNames[i].Class.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - 5, 60));

		if ( ImGui::BeginDragDropSource(ImGuiDragDropFlags_None) )
		{
			Drop drop = { i, new std::filesystem::path(m_ScriptPaths[i]) };
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &drop, sizeof(Drop));
			ImGui::Text("%s", m_ScriptNames[i].Class.c_str());
			ImGui::EndDragDropSource();
		}
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL") )
			{
				IM_ASSERT(payload->DataSize == sizeof(Drop));
				const auto drop = *static_cast<Drop *>(payload->Data);
				std::swap(m_ScriptNames[i], m_ScriptNames[drop.StorageIndex]);
				std::swap(m_ScriptPaths[i], m_ScriptPaths[drop.StorageIndex]);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void ScriptManager::SyncScriptPaths()
{
	const size_t noScripts = FileIOManager::GetFileCount(m_ScriptFolderPath, ".cs");

	if ( noScripts > m_ScriptPaths.size() )
	{
		m_ScriptPaths.clear();
		m_ScriptNames.clear();
		m_ScriptPaths = FileIOManager::GetFiles(m_ScriptFolderPath, ".cs");
		std::for_each(m_ScriptPaths.begin(), m_ScriptPaths.end(), [&](const fs::path &path) mutable
					  {
						  m_ScriptNames.push_back({ "Script", path.stem().string() });
					  });
	}
}

#define ROW(str) oss << (#str) << ";\n"
#define CLASS(name) oss << "class " << (name) << ";\n"
#define NAMESPACE(name) oss << "namespace " << (name) << ";\n"
#define EMPTY_ROW oss << "\n"
#define OPEN_BRACET oss << "{\n"
#define CLOSE_BRACET oss << "}\n"
#define FUNCTION(return, name) oss << (#return) << " " << (#name) << "\n{\n}\n"

bool ScriptManager::CreateScript(const std::string &namespaceName,
								 const std::string &className)
{
	std::ostringstream oss;
	ROW(using System);
	ROW(using System.Collections.Generic);
	ROW(using System.Linq);
	ROW(using System.Text);
	ROW(using System.Threading.Tasks);
	EMPTY_ROW;
	ROW(using Se);
	EMPTY_ROW;
	NAMESPACE(namespaceName);
	OPEN_BRACET;
	CLASS(className);
	OPEN_BRACET;
	FUNCTION(void, OnCreate);
	FUNCTION(void, OnDestroy);
	FUNCTION(void, OnUpdate);
	CLOSE_BRACET;
	CLOSE_BRACET;

	const std::string path = "../ExampleApp/Src/" + className + ".cs";
	FileIOManager::Write(reinterpret_cast<const Uint8 *>(oss.str().data()), oss.str().size(), path);

	return true;
}
}


#define SAFFRON_ENTRY_POINT

#include "EditorApplication.h"

namespace Se
{
App* Se::CreateApplication()
{
	return new EditorApplication({"Saffron Engine", 1700, 900});
}

EditorApplication::EditorApplication(const Properties& props) :
	App(props)
{
	/*const auto wantProjectSelectorFn = [this] {
		Run::Later([=] {
			PopLayer();
			m_PreLoader->Reset();
			m_EditorLayer.reset();
			PushLayer(m_StartupLayer);
				   });
	};
	const auto projectSelectFn = [wantProjectSelectorFn, this](const std::shared_ptr<Project> &project) {
		Run::Later([=] {
			PopLayer();
			m_PreLoader->Reset();
			m_EditorLayer = CreateShared<EditorLayer>(project);
			m_EditorLayer->GetSignal(EditorLayer::Signals::OnWantProjectSelector).Connect(wantProjectSelectorFn);
			PushLayer(m_EditorLayer);
				   });
	};*/

	//m_StartupLayer->GetSignal(StartupLayer::Signals::OnProjectSelect).Connect(projectSelectFn);


	// TEMP
	//auto project = CreateShared<Project>("C:/Users/ownem/source/repos/SaffronEngine/Games/2DGameProject/2DGameProject.spr");
	//m_EditorLayer = CreateShared<EditorLayer>(project);
	// ----

	_projectLayer = CreateShared<ProjectLayer>();
}

void EditorApplication::OnInit()
{
	PushLayer(_projectLayer);
}

void EditorApplication::OnUpdate()
{
}

void EditorApplication::OnShutdown()
{
}
}

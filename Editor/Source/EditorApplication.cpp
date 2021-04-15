#include "SaffronPCH.h"

#define SAFFRON_ENTRY_POINT

#include "EditorApplication.h"

namespace Se
{
App* Se::CreateApplication()
{
	return new EditorApplication({"Saffron Engine", 1700, 900});
}

EditorApplication::EditorApplication(const Properties& props) :
	App(props),
	m_StartupLayer(CreateShared<StartupLayer>())
{
	const auto wantProjectSelectorFn = [this]
	{
		Run::Later([=]
		{
			PopLayer();
			m_PreLoader->Reset();
			m_EditorLayer.reset();
			PushLayer(m_StartupLayer);
		});
		return false;
	};
	const auto projectSelectFn = [wantProjectSelectorFn, this](const std::shared_ptr<Project>& project)
	{
		Run::Later([=]
		{
			PopLayer();
			m_PreLoader->Reset();
			m_EditorLayer = CreateShared<EditorLayer>(project);
			m_EditorLayer->WantProjectSelector += wantProjectSelectorFn;
			PushLayer(m_EditorLayer);
		});
		return false;
	};

	m_StartupLayer->ProjectSelected += projectSelectFn;


	// TEMP
	auto project = CreateShared<Project>(
		"C:/Users/ownem/source/repos/SaffronEngine/Games/2DGameProject/2DGameProject.spr");
	m_EditorLayer = CreateShared<EditorLayer>(project);
	// ----
}

void EditorApplication::OnInit()
{
	PushLayer(m_EditorLayer);
}

void EditorApplication::OnUpdate()
{
}
}

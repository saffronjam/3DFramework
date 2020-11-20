#define SAFFRON_ENTRY_POINT
#include <Saffron.h>

#include "SaffronBun.h"


Application *Se::CreateApplication()
{
	return new SaffronBunApplication({ "Saffron Bun", 1600, 900 });
}

SaffronBunApplication::SaffronBunApplication(const Properties &props)
	:
	Application(props),
	m_StartupLayer(Shared<StartupLayer>::Create())
{
	const auto wantProjectSelectorFn = [this]
	{
		Run::Later([=]
				   {
					   PopLayer();
					   m_PreLoader->Reset();
					   m_EditorLayer.Reset();
					   PushLayer(m_StartupLayer);
				   });
	};
	const auto projectSelectFn = [wantProjectSelectorFn, this](const Shared<Project> &project)
	{
		Run::Later([=]
				   {
					   PopLayer();
					   m_PreLoader->Reset();
					   m_EditorLayer = Shared<EditorLayer>::Create(project);
					   m_EditorLayer->GetSignal(EditorLayer::Signals::OnWantProjectSelector).Connect(wantProjectSelectorFn);
					   PushLayer(m_EditorLayer);
				   });
	};

	m_StartupLayer->GetSignal(StartupLayer::Signals::OnProjectSelect).Connect(projectSelectFn);


	// TEMP
	auto project = Shared<Project>::Create("Projects/2DGameProject/2DGameProject.spr");
	m_EditorLayer = Shared<EditorLayer>::Create(project);
	// ----
}

void SaffronBunApplication::OnInit()
{

	PushLayer(m_EditorLayer);
}

void SaffronBunApplication::OnUpdate()
{
}

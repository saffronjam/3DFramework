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
	_startupLayer(Shared<StartupLayer>::Create())
{
	const auto wantProjectSelectorFn = [this]
	{
		Run::Later([=]
		{
			PopLayer();
			_preLoader->Reset();
			_editorLayer.Reset();
			PushLayer(_startupLayer);
		});
		return false;
	};
	const auto projectSelectFn = [wantProjectSelectorFn, this](const Shared<Project>& project)
	{
		Run::Later([=]
		{
			PopLayer();
			_preLoader->Reset();
			_editorLayer = Shared<EditorLayer>::Create(project);
			_editorLayer->WantProjectSelector += wantProjectSelectorFn;
			PushLayer(_editorLayer);
		});
		return false;
	};

	_startupLayer->ProjectSelected += projectSelectFn;


	// TEMP
	auto project = Shared<Project>::Create(
		"C:/Users/ownem/source/repos/SaffronEngine/Games/2DGameProject/2DGameProject.spr");
	_editorLayer = Shared<EditorLayer>::Create(project);
	// ----
}

void EditorApplication::OnInit()
{
	PushLayer(_editorLayer);
}

void EditorApplication::OnUpdate()
{
}
}

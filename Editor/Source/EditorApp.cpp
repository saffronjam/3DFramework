#include <memory>

#define SE_ENTRY_POINT

#include "EditorApp.h"

namespace Se
{
std::unique_ptr<App> CreateApp()
{
	return std::make_unique<EditorApp>(AppSpec{WindowSpec{.Title="Hello, world"}});
}

EditorApp::EditorApp(const AppSpec& spec) :
	App(spec),
	_editorLayer(std::make_shared<EditorLayer>())
{
}

void EditorApp::OnCreate()
{
	AttachLayer(_editorLayer);
}

void EditorApp::OnExit()
{
}
}

#include "Layers/ProjectLayer.h"

namespace Se
{
void ProjectLayer::OnAttach(std::shared_ptr<BatchLoader>& loader)
{
	BaseLayer::OnAttach(loader);
	
	_scene = CreateShared<EditorScene>();
	_editorCamera = CreateShared<EditorCamera>();
	_scene->SetProjectionMatrix(_editorCamera->GetProjectionMatrix());
}

void ProjectLayer::OnDetach()
{
	BaseLayer::OnDetach();
}

void ProjectLayer::OnUpdate()
{
	BaseLayer::OnUpdate();

	_editorCamera->OnUpdate();
	_scene->SetViewMatrix(_editorCamera->GetViewMatrix());

	_scene->OnUpdate();
	_scene->OnRender();

	//ImGui::Begin("Test");
	//ImGui::Image(reinterpret_cast<ImTextureID>(&_cubeTexture->GetNativeHandle()), ImGui::GetWindowSize());
	//ImGui::End();
}

void ProjectLayer::OnGuiRender()
{
	BaseLayer::OnGuiRender();

	_scene->OnGuiRender();
}
}

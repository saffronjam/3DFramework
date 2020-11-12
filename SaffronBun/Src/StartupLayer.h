#pragma once

#include <Saffron.h>

using namespace Se;

class StartupLayer : public Layer
{
public:
	StartupLayer();

	void OnAttach(Shared<BatchLoader> &loader) override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnGuiRender() override;
	void OnEvent(const Event &event) override;

	void SetOnProjectSelectCallback(const Function<void(const Application::Project &)> &onProjectSelect) { m_OnProjectSelect = onProjectSelect; }

private:
	Map<String, Shared<Texture2D>> m_TextureStore;
	const Application::Project *m_HoveredProject = nullptr;
	DateTime m_Today;
	Function<void(const Application::Project &)> m_OnProjectSelect;

};

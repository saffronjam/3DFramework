#pragma once

#include <Saffron.h>

namespace Se
{
class StartupLayer : public Layer
{
public:
	StartupLayer();

	void OnAttach(Shared<BatchLoader>& loader) override;

	void OnDetach() override;

	void OnUpdate() override;

	void OnGuiRender() override;

	void OnEvent(const Event& event) override;

public:
	EventSubscriberList<const Shared<Project>&> ProjectSelected;

private:
	Map<String, Shared<Texture2D>> m_TextureStore;
	Shared<Project> m_SelectedProject = nullptr;
	DateTime m_Today;
	Optional<Shared<Project>> m_NewProject;
};
}

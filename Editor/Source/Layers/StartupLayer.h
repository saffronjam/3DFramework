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
	TreeMap<String, Shared<Texture2D>> _textureStore;
	Shared<Project> _selectedProject = nullptr;
	DateTime _today;
	Optional<Shared<Project>> _newProject;
};
}

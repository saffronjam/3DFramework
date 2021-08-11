#pragma once

#include <Saffron.h>

namespace Se
{
class EditorLayer : public Layer
{
public:
	EditorLayer();

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate(TimeSpan ts) override;

	void OnUi() override;

private:
	EditorCamera _camera;
	Scene _scene;

	// Ui
	DockSpacePanel _dockSpacePanel;
	ViewportPanel _viewportPanel;
	ViewportPanel _depthViewportPanel;
};
}

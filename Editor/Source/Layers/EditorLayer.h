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

	// Temporary
	std::shared_ptr<Framebuffer> _framebuffer;
	std::shared_ptr<DepthStencil> _depthStencil;
	std::shared_ptr<MvpCBuffer> _mvpCBuffer;
	std::shared_ptr<Mesh> _mesh;

	Viewport _viewport;
	PrimitiveTopology _topology = PrimitiveTopologyType::TriangleList;
		
	// Ui
	DockSpacePanel _dockSpacePanel;
	ViewportPanel _viewportPanel;
	ViewportPanel _depthViewportPanel;
};
}

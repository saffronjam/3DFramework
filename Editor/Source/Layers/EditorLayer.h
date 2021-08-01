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

	void OnUpdate(TimeSpan dt) override;

	void OnUi() override;

private:
	// Temporary
	std::shared_ptr<Framebuffer> _framebuffer;
	std::shared_ptr<InputLayout> _layout;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<VertexBuffer> _vertexBuffer;
	Viewport _viewport;
	PrimitiveTopology _topology = PrimitiveTopologyType::TriangleList;

	// Ui
	DockSpacePanel _dockSpacePanel;
	ViewportPanel _viewportPanel;
};
}

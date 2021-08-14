#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/RenderState.h"

namespace Se
{
class LinePass : public RenderPass
{
public:
	explicit LinePass(const std::string& name, struct SceneCommon& sceneCommon);

	void Execute() override;

private:
	std::shared_ptr<Framebuffer> _target;

	std::shared_ptr<Shader> _shader;
	std::shared_ptr<InputLayout> _inputLayout;
	std::shared_ptr<VertexBuffer> _vertexBuffer;
	std::shared_ptr<MvpCBuffer> _mvpCBuffer;

	const RenderState _renderState = RenderState::DepthTest_Less | RenderState::Rasterizer_CullFront |
		RenderState::Rasterizer_Fill | RenderState::Topology_LineList;
};
}

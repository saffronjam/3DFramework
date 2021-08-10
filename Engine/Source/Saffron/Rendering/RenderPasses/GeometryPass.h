#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Bindables/MvpCBuffer.h"

namespace Se
{
class GeometryPass : public RenderPass
{
public:
	explicit GeometryPass(const std::string& name, struct SceneCommon& sceneCommon);

	void Execute() override;

	void SetViewportSize(uint width, uint height) override;

private:
	std::shared_ptr<Framebuffer> _target;
	std::shared_ptr<MvpCBuffer> _mvpCBuffer;
};
}

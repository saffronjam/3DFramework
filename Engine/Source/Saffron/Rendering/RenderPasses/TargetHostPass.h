#pragma once

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class TargetHostPass : public RenderPass
{
public:
	explicit TargetHostPass(const std::string& name, struct SceneCommon& sceneCommon);

	void Execute() override;

private:

	std::shared_ptr<Framebuffer> _target;
};
}

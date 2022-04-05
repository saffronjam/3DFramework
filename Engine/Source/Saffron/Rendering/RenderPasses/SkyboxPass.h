#pragma once

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
namespace ShaderStructs
{
struct alignas(16) SkyboxCameraCBuffer
{
	Matrix ViewProj;
	Matrix ViewProjInv;
};
}
class SkyboxPass : public RenderPass
{
public:
	explicit SkyboxPass(const std::string& name, struct SceneCommon& sceneCommon);

	void Execute() override;
	
private:
	std::shared_ptr<Framebuffer> _target;
};
}

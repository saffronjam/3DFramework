#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class GeometryPass : public RenderPass
{
public:
	explicit GeometryPass(String name);

	void Execute() override;

	void OnViewportResize(Uint32 width, Uint32 height) override;

private:
	Shared<Framebuffer> _framebuffer;
	Array<Shared<Framebuffer>, 4> _shadowMaps;
	RendererID _shadowMapSampler;
	Shared<Texture2D> _brdflut;
};
}

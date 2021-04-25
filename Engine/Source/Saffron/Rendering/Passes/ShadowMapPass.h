#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
class ShadowMapPass : public RenderPass
{
private:
	struct CascadeData
	{
		Matrix4 ViewProj;
		Matrix4 View;
		float SplitDepth;
	};

	struct FrustumBounds
	{
		float r, l, b, t, f, n;
	};

public:
	explicit ShadowMapPass(String name);

	void Execute() override;
	void OnViewportResize(uint width, uint height) override;

private:
	void CalculateCascades(CascadeData* cascades, const Vector3& lightDirection);

private:
	Array<Shared<Framebuffer>, 4> _shadowMaps;
	Shared<Shader> _shader;
};
}

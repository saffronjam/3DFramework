#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"
#include "Saffron/Rendering/Bindables/VertexBuffer.h"
#include "Saffron/Rendering/Bindables/InputLayout.h"

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

struct CubePrimitive
{
	std::shared_ptr<VertexBuffer> Vb;
	std::shared_ptr<IndexBuffer> Ib;
	std::shared_ptr<InputLayout> Il;
};

class SkyboxPass : public RenderPass
{
public:
	explicit SkyboxPass(const std::string& name, struct SceneCommon& sceneCommon);

	void Execute() override;

private:


private:
	std::shared_ptr<Framebuffer> _target;
	std::shared_ptr<Shader> _shader;
	std::shared_ptr<ConstantBuffer<ShaderStructs::SkyboxCameraCBuffer>> _skyboxCBuffer;

	CubePrimitive _cubePrimitive;
};
}

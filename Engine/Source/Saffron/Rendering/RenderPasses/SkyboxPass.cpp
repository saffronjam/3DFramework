#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/SkyboxPass.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Binders/Binder.h"
#include "Saffron/Rendering/Binders/ShaderBindFlagsBinder.h"
#include "Saffron/Rendering/Binders/TextureUsageBinder.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/VertexStorage.h"
#include "Saffron/Rendering/VertexTypes.h"

namespace Se
{
SkyboxPass::SkyboxPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon)
{
	RegisterInput("Target", _target);
	RegisterOutput("Target", _target);

	//// Setup unit cube
	VertexLayout layout = PosVertex::Layout();
	VertexStorage storage(layout);
	constexpr int side = 1.0f;
	// Maaaaybe should be inside a generator class
	storage.Add<PosVertex>({Vector3{-side, -side, -side}}); // 0 near side
	storage.Add<PosVertex>({Vector3{side, -side, -side}}); // 1
	storage.Add<PosVertex>({Vector3{-side, side, -side}}); // 2
	storage.Add<PosVertex>({Vector3{side, side, -side}}); // 3
	storage.Add<PosVertex>({Vector3{-side, -side, side}}); // 4 far side
	storage.Add<PosVertex>({Vector3{side, -side, side}}); // 5
	storage.Add<PosVertex>({Vector3{-side, side, side}}); // 6
	storage.Add<PosVertex>({Vector3{side, side, side}}); // 7
	storage.Add<PosVertex>({Vector3{-side, -side, -side}}); // 8 left side
	storage.Add<PosVertex>({Vector3{-side, side, -side}}); // 9
	storage.Add<PosVertex>({Vector3{-side, -side, side}}); // 10
	storage.Add<PosVertex>({Vector3{-side, side, side}}); // 11
	storage.Add<PosVertex>({Vector3{side, -side, -side}}); // 12 right side
	storage.Add<PosVertex>({Vector3{side, side, -side}}); // 13
	storage.Add<PosVertex>({Vector3{side, -side, side}}); // 14
	storage.Add<PosVertex>({Vector3{side, side, side}}); // 15
	storage.Add<PosVertex>({Vector3{-side, -side, -side}}); // 16 bottom side
	storage.Add<PosVertex>({Vector3{side, -side, -side}}); // 17
	storage.Add<PosVertex>({Vector3{-side, -side, side}}); // 18
	storage.Add<PosVertex>({Vector3{side, -side, side}}); // 19
	storage.Add<PosVertex>({Vector3{-side, side, -side}}); // 20 top side
	storage.Add<PosVertex>({Vector3{side, side, -side}}); // 21
	storage.Add<PosVertex>({Vector3{-side, side, side}}); // 22
	storage.Add<PosVertex>({Vector3{side, side, side}}); // 23
}

void SkyboxPass::Execute()
{
	Renderer::SetViewportSize(_target->Width(), _target->Height());
	Renderer::SetRenderState(
		RenderState::DepthTest_LessEqual | RenderState::DepthWriteMask_All | RenderState::Rasterizer_CullNever |
		RenderState::Rasterizer_Fill | RenderState::Topology_TriangleList);

	const auto& com = SceneCommon();
}
}

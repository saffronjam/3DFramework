#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/SkyboxPass.h"

#include "Saffron/Rendering/Binders/Binder.h"
#include "Saffron/Rendering/Binders/ShaderBindFlagsBinder.h"
#include "Saffron/Rendering/Binders/TextureUsageBinder.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/VertexTypes.h"

namespace Se
{
SkyboxPass::SkyboxPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon)
{
	RegisterInput("Target", _target);
	RegisterOutput("Target", _target);

	_shader = Shader::Create("Skybox");
	_skyboxCBuffer = ConstantBuffer<ShaderStructs::SkyboxCameraCBuffer>::Create();

	//// Setup unit cube
	VertexLayout layout = PosVertex::Layout();
	VertexStorage storage(layout);
	constexpr int side = 1.0f;
	// Maaaaybe should be inside a generator class
	storage.Add<PosVertex>({Vector3{-side, -side, -side}}); // 0 near side
	storage.Add<PosVertex>({Vector3{side, -side, -side}}); // 1
	storage.Add<PosVertex>({Vector3{-side, side, -side}}); // 2
	storage.Add<PosVertex>({Vector3{side, side, -side} }); // 3
	storage.Add<PosVertex>({Vector3{-side, -side, side}}); // 4 far side
	storage.Add<PosVertex>({Vector3{side, -side, side} }); // 5
	storage.Add<PosVertex>({Vector3{-side, side, side} }); // 6
	storage.Add<PosVertex>({Vector3{side, side, side} }); // 7
	storage.Add<PosVertex>({Vector3{-side, -side, -side}}); // 8 left side
	storage.Add<PosVertex>({Vector3{-side, side, -side}}); // 9
	storage.Add<PosVertex>({Vector3{-side, -side, side}}); // 10
	storage.Add<PosVertex>({Vector3{-side, side, side} }); // 11
	storage.Add<PosVertex>({Vector3{side, -side, -side}}); // 12 right side
	storage.Add<PosVertex>({Vector3{side, side, -side} }); // 13
	storage.Add<PosVertex>({Vector3{side, -side, side} }); // 14
	storage.Add<PosVertex>({Vector3{side, side, side} }); // 15
	storage.Add<PosVertex>({Vector3{-side, -side, -side}}); // 16 bottom side
	storage.Add<PosVertex>({Vector3{side, -side, -side}}); // 17
	storage.Add<PosVertex>({Vector3{-side, -side, side}}); // 18
	storage.Add<PosVertex>({Vector3{side, -side, side} }); // 19
	storage.Add<PosVertex>({Vector3{-side, side, -side}}); // 20 top side
	storage.Add<PosVertex>({Vector3{side, side, -side} }); // 21
	storage.Add<PosVertex>({Vector3{-side, side, side} }); // 22
	storage.Add<PosVertex>({Vector3{side, side, side}}); // 23
	_cubePrimitive.Vb = VertexBuffer::Create(storage);
	_cubePrimitive.Ib = IndexBuffer::Create(
		{
			0, 2, 1, 2, 3, 1, 4, 5, 7, 4, 7, 6, 8, 10, 9, 10, 11, 9, 12, 13, 15, 12, 15, 14, 16, 17, 18, 18, 17, 19, 20,
			23, 21, 20, 22, 23
		}
	);

	_cubePrimitive.Il = InputLayout::Create(layout, _shader);
}

void SkyboxPass::Execute()
{
	Renderer::SetViewportSize(_target->Width(), _target->Height());
	Renderer::SetRenderState(
		RenderState::DepthTest_LessEqual | RenderState::DepthWriteMask_All | RenderState::Rasterizer_CullNever |
		RenderState::Rasterizer_Fill | RenderState::Topology_TriangleList
	);

	const auto& com = SceneCommon();

	ScopedBinder(_target);

	const auto viewProj = com.CameraData.View * com.CameraData.Projection;
	_skyboxCBuffer->Update({viewProj.Transpose(), viewProj.Transpose().Invert()});

	ScopedBindFlagsBinder(com.Environment->Texture(), BindFlag_PS);
	ScopedTexUsageBinder(com.Environment->Texture(), TextureUsage_ShaderResource);

	ScopedBinder(com.Environment->Texture());
	ScopedBinder(_shader);
	ScopedBinder(_skyboxCBuffer);
	ScopedBinder(_cubePrimitive.Ib);
	ScopedBinder(_cubePrimitive.Vb);
	ScopedBinder(_cubePrimitive.Il);

	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			constexpr int nCubeIndices = 36;
			package.Context.DrawIndexed(nCubeIndices, 0, 0);
		}
	);
}
}

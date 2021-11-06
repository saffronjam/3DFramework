#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/SkyboxPass.h"
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
	VertexLayout layout = PosTexVertex::Layout();
	VertexStorage storage(layout);
	constexpr int side = 1.0f;
	// Maaaaybe should be inside a generator class
	storage.Add<PosTexVertex>({Vector3{-side, -side, -side}, Vector2{0.0f, 0.0f}}); // 0 near side
	storage.Add<PosTexVertex>({Vector3{side, -side, -side}, Vector2{1.0f, 0.0f}}); // 1
	storage.Add<PosTexVertex>({Vector3{-side, side, -side}, Vector2{0.0f, 1.0f}}); // 2
	storage.Add<PosTexVertex>({Vector3{side, side, -side}, Vector2{1.0f, 1.0f}}); // 3
	storage.Add<PosTexVertex>({Vector3{-side, -side, side}, Vector2{0.0f, 0.0f}}); // 4 far side
	storage.Add<PosTexVertex>({Vector3{side, -side, side}, Vector2{1.0f, 0.0f}}); // 5
	storage.Add<PosTexVertex>({Vector3{-side, side, side}, Vector2{0.0f, 1.0f}}); // 6
	storage.Add<PosTexVertex>({Vector3{side, side, side}, Vector2{1.0f, 1.0f}}); // 7
	storage.Add<PosTexVertex>({Vector3{-side, -side, -side}, Vector2{0.0f, 0.0f}}); // 8 left side
	storage.Add<PosTexVertex>({Vector3{-side, side, -side}, Vector2{1.0f, 0.0f}}); // 9
	storage.Add<PosTexVertex>({Vector3{-side, -side, side}, Vector2{0.0f, 1.0f}}); // 10
	storage.Add<PosTexVertex>({Vector3{-side, side, side}, Vector2{1.0f, 1.0f}}); // 11
	storage.Add<PosTexVertex>({Vector3{side, -side, -side}, Vector2{0.0f, 0.0f}}); // 12 right side
	storage.Add<PosTexVertex>({Vector3{side, side, -side}, Vector2{1.0f, 0.0f}}); // 13
	storage.Add<PosTexVertex>({Vector3{side, -side, side}, Vector2{0.0f, 1.0f}}); // 14
	storage.Add<PosTexVertex>({Vector3{side, side, side}, Vector2{1.0f, 1.0f}}); // 15
	storage.Add<PosTexVertex>({Vector3{-side, -side, -side}, Vector2{0.0f, 0.0f}}); // 16 bottom side
	storage.Add<PosTexVertex>({Vector3{side, -side, -side}, Vector2{1.0f, 0.0f}}); // 17
	storage.Add<PosTexVertex>({Vector3{-side, -side, side}, Vector2{0.0f, 1.0f}}); // 18
	storage.Add<PosTexVertex>({Vector3{side, -side, side}, Vector2{1.0f, 1.0f}}); // 19
	storage.Add<PosTexVertex>({Vector3{-side, side, -side}, Vector2{0.0f, 0.0f}}); // 20 top side
	storage.Add<PosTexVertex>({Vector3{side, side, -side}, Vector2{1.0f, 0.0f}}); // 21
	storage.Add<PosTexVertex>({Vector3{-side, side, side}, Vector2{0.0f, 1.0f}}); // 22
	storage.Add<PosTexVertex>({Vector3{side, side, side}, Vector2{1.0f, 1.0f}}); // 23
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

	_target->Bind();

	const auto viewProj = SceneCommon().CameraData.View * SceneCommon().CameraData.Projection;
	_skyboxCBuffer->Update({viewProj.Transpose(), viewProj.Invert().Transpose()});

	SceneCommon().Environment->Bind();

	_shader->Bind();
	_skyboxCBuffer->Bind();
	_cubePrimitive.Ib->Bind();
	_cubePrimitive.Vb->Bind();
	_cubePrimitive.Il->Bind();

	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			constexpr int nCubeIndices = 36;
			package.Context.DrawIndexed(nCubeIndices, 0, 0);
		}
	);


	_target->Unbind();
}
}

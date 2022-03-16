#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/ShadowMapPass.h"

#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

#undef min
#undef max

namespace Se
{
ShadowMapPass::ShadowMapPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon),
	_shadowMap(Framebuffer::Create({Width, Height, {{ImageFormat::RGBA, 6}, {ImageFormat::Depth, 6}}})),
	_shadowShader(Shader::Create("Shadow")),
	_depthTextureShader(Shader::Create("DepthTexture")),
	_shadowMapCBuffer(ConstantBuffer<ShadowMapPassCBuffer>::Create())
{
	_shadowMapCBuffer->SetBindFlags(BindFlag_GS | BindFlag_VS | BindFlag_PS);

	//_shadowMap->SetDepthClearValue(0.0f);

	RegisterOutput("Target0", _shadowMap);
}

void ShadowMapPass::OnSetupFinished()
{
}

void ShadowMapPass::OnUi()
{
	ImGui::Begin("Shadow map");
	ImGui::Checkbox("Orthographic", &_orthographic);
	Ui::Image(_shadowMap->ImageByIndex(0), *_depthTextureShader, {500.0f, 500.0f});
	ImGui::End();
}

void ShadowMapPass::Execute()
{
	_shadowMap->Bind();
	_shadowMap->Clear();

	/*Renderer::SetRenderState(
		RenderState::DepthTest_Greter | RenderState::DepthWriteMask_All | RenderState::Rasterizer_CullFront |
		RenderState::Rasterizer_Fill | RenderState::Topology_TriangleList
	);*/

	for (auto& drawCommand : SceneCommon().DrawCommands.at(RenderChannel_Shadow))
	{
		drawCommand.Model->Bind();

		// Overwrite shader
		_shadowShader->Bind();

		Renderer::SetViewportSize(Width, Height);

		Renderer::Submit(
			[this, drawCommand](const RendererPackage& package) mutable
			{
				auto& common = SceneCommon();

				const auto& pos = common.PointLight.Position;
				const auto proj = Matrix::CreatePerspectiveFieldOfView(
					Math::Pi / 2.0f,
					1.0f,
					0.5f,
					common.PointLight.Radius
				);

				std::array<Matrix, 6> lookAts;
				lookAts[0] = Matrix::CreateLookAt(pos, pos + Vector3{1.0, 0.0, 0.0}, Vector3{0.0, 1.0, 0.0}) * proj;
				lookAts[1] = Matrix::CreateLookAt(pos, pos + Vector3{-1.0, 0.0, 0.0}, Vector3{0.0, 1.0, 0.0}) * proj;
				lookAts[2] = Matrix::CreateLookAt(pos, pos + Vector3{0.0, 1.0, 0.0}, Vector3{0.0, 0.0, 1.0}) * proj;
				lookAts[3] = Matrix::CreateLookAt(pos, pos + Vector3{0.0, -1.0, 0.0}, Vector3{0.0, 0.0, -1.0}) * proj;
				lookAts[5] = Matrix::CreateLookAt(pos, pos + Vector3{0.0, 0.0, 1.0}, Vector3{0.0, 1.0, 0.0}) * proj;
				lookAts[4] = Matrix::CreateLookAt(pos, pos + Vector3{0.0, 0.0, -1.0}, Vector3{0.0, 1.0, 0.0}) * proj;

				for (int i = 0; i < 6; i++)
				{
					lookAts[i] = lookAts[i].Transpose();
				}

				common.PointLight.LookAt = lookAts;

				_shadowMapCBuffer->Bind();

				for (const auto& submesh : drawCommand.Model->SubMeshes())
				{
					const auto modelTransform = submesh.Transform * drawCommand.Model->Transform() * drawCommand.
						Transform;

					_shadowMapCBuffer->Update(
						{Color{1.0f, 0.0f, 0.0f, 1.0f}, modelTransform.Transpose(), common.PointLight}
					);
					_shadowMapCBuffer->UploadData();
					package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
				}

				_shadowMapCBuffer->Unbind();
			}
		);

		_shadowShader->Unbind();
	}

	Renderer::ResetRenderState();

	_shadowMap->Unbind();
}
}

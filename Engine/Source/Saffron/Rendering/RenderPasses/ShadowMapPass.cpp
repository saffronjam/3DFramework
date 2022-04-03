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
	_shadowMap(sceneCommon.LightRegister.Atlas()),
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
	Ui::Image(_shadowMap->DepthImage(), _depthTextureShader, {500.0f, 500.0f});
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

	for (int i = 0; i < 6; i++)
	{
		for (auto& drawCommand : SceneCommon().DrawCommands.at(RenderChannel_Shadow))
		{
			drawCommand.Model->Bind();

			// Overwrite shader
			_shadowShader->Bind();

			Renderer::SetViewportSize(
				LightRegister::SmWidth * i,
				0,
				LightRegister::SmWidth,
				LightRegister::SmHeight
			);

			Renderer::Submit(
				[this, drawCommand, i](const RendererPackage& package) mutable
				{
					auto& common = SceneCommon();


					// Common projection for each point light
					const auto proj = Matrix::CreatePerspectiveFieldOfView(
						Math::Pi / 2.0f,
						1.0f,
						0.5f,
						common.PointLight.Radius
					);

					// for(each point light in scene)


					// Per point light

					/*
					typedef enum D3D11_TEXTURECUBE_FACE {
					  D3D11_TEXTURECUBE_FACE_POSITIVE_X = 0,
					  D3D11_TEXTURECUBE_FACE_NEGATIVE_X = 1,
					  D3D11_TEXTURECUBE_FACE_POSITIVE_Y = 2,
					  D3D11_TEXTURECUBE_FACE_NEGATIVE_Y = 3,
					  D3D11_TEXTURECUBE_FACE_POSITIVE_Z = 4,
					  D3D11_TEXTURECUBE_FACE_NEGATIVE_Z = 5
					} ;
					*/


					// Calculate sub-area of shadow map

					_shadowMapCBuffer->Bind();

					for (const auto& submesh : drawCommand.Model->SubMeshes())
					{
						const auto modelTransform = submesh.Transform * drawCommand.Model->Transform() * drawCommand.
							Transform;

						const auto mvp = modelTransform * common.PointLightShaderStruct.ViewProjs[i];

						_shadowMapCBuffer->Update(
							{
								Color{1.0f, 0.0f, 0.0f, 1.0f}, modelTransform.Transpose(), mvp.Transpose(),
								common.PointLightShaderStruct.Position, common.PointLightShaderStruct.Radius
							}
						);
						_shadowMapCBuffer->UploadData();
						package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
					}

					_shadowMapCBuffer->Unbind();
				}
			);
		}
	}

	_shadowShader->Unbind();

	Renderer::ResetRenderState();

	_shadowMap->Unbind();
}
}

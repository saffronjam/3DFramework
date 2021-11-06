﻿#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/GeometryPass.h"

#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
GeometryPass::GeometryPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon),
	_pointLightCBuffer(ConstantBuffer<PointLightCBuffer>::Create({}, 2)),
	_mvpCBuffer(TransformCBuffer::Create()),
	_shadowMapTexture(Texture::Create(TextureSpec{SamplerWrap::Border, SamplerFilter::Bilinear}, 4))
{
	RegisterInput("ShadowMap0", _shadowMap);
	RegisterInput("Target", _target);
	RegisterOutput("Target", _target);

	_pointLightCBuffer->SetBindFlags(ConstantBufferBindFlags_PS | ConstantBufferBindFlags_VS);
}

void GeometryPass::OnSetupFinished()
{
	Renderer::Submit(
		[this](const RendererPackage&)
		{
			_shadowMapTexture->SetImage(_shadowMap->DepthImagePtr());
		}
	);
}

void GeometryPass::OnUi()
{
}

void GeometryPass::Execute()
{
	auto& common = SceneCommon();

	// Setup renderer
	Renderer::ResetRenderState();
	Renderer::SetViewportSize(_target->Width(), _target->Height());

	// Update point light data
	PointLightCBuffer data;
	data.PointLights[0] = common.PointLight;
	data.PointLights[0].LightTransform = data.PointLights[0].LightTransform.Transpose();
	data.nPointLights = 1;
	_pointLightCBuffer->Update(data);

	// Render to texture
	_target->Bind();

	for (auto& drawCommand : common.DrawCommands.at(RenderChannel_Geometry))
	{
		drawCommand.Model->Bind();
		_pointLightCBuffer->Bind();
		common._sceneCommonCBuffer->Update({ common.CameraData.Position });
		common._sceneCommonCBuffer->Bind();
		
		Renderer::Submit(
			[this, drawCommand, common](const RendererPackage& package)
			{
				_mvpCBuffer->Bind();

				const auto& materials = drawCommand.Model->Materials();
				const auto viewProj = common.CameraData.View * common.CameraData.Projection;

				for (const auto& submesh : drawCommand.Model->SubMeshes())
				{
					const auto& srvs = drawCommand.Model->MaterialTextureShaderViews(submesh.MaterialIndex);
					package.Context.PSSetShaderResources(0, srvs.size(), srvs.data());

					materials[submesh.MaterialIndex]->CBuffer().Bind();
					materials[submesh.MaterialIndex]->Shader().Bind();
					_shadowMapTexture->Bind();
					
					const auto modelTransform = submesh.Transform * drawCommand.Model->Transform() * drawCommand.
						Transform;

					_mvpCBuffer->Update(
						{modelTransform, modelTransform * common.CameraData.View, viewProj, modelTransform * viewProj}
					);
					_mvpCBuffer->UploadData();
					package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
				}

				_mvpCBuffer->Unbind();
			}
		);

		common._sceneCommonCBuffer->Unbind();
		drawCommand.Model->Unbind();
	}

	_shadowMapTexture->Unbind();
	_target->Unbind();
}


void GeometryPass::SetViewportSize(uint width, uint height)
{
	_target->Resize(width, height);
}
}

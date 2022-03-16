#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/GeometryPass.h"

#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/Binders/Binder.h"

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

	_pointLightCBuffer->SetBindFlags(BindFlag_PS | BindFlag_VS);
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
	data.nPointLights = 1;
	_pointLightCBuffer->Update(data);

	// Render to texture
	ScopedBinder(_target);

	for (auto& drawCommand : common.DrawCommands.at(RenderChannel_Geometry))
	{
		common._sceneCommonCBuffer->Update({common.CameraData.Position});

		ScopedBinder(drawCommand.Model);
		ScopedBinder(_pointLightCBuffer);
		ScopedBinder(common._sceneCommonCBuffer);

		Renderer::Submit(
			[this, drawCommand, common](const RendererPackage& package)
			{
				Renderer::BeginStrategy(RenderStrategy::Immediate);

				ScopedBinder(_mvpCBuffer);

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

				Renderer::EndStrategy();
			}
		);
	}

	_shadowMapTexture->Unbind();
}


void GeometryPass::SetViewportSize(uint width, uint height)
{
	_target->Resize(width, height);
}
}

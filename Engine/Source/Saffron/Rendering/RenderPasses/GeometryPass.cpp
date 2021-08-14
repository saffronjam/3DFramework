#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/GeometryPass.h"

#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
GeometryPass::GeometryPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon),
	_target(Framebuffer::Create(FramebufferSpec{500, 500, {ImageFormat::RGBA, ImageFormat::Depth24Stencil8}})),
	_pointLightCBuffer(ConstantBuffer<PointLightCBuffer>::Create({}, 2)),
	_mvpCBuffer(TransformCBuffer::Create()),
	_shadowMapTexture(Texture::Create(1)),
	_shadowSampler(Sampler::Create({1, SamplerEdge::Border, SamplerFilter::Bilinear, Color{1.0f, 1.0f, 1.0f, 1.0f}})),
	_testCube(TextureCube::Create(100, 100, ImageFormat::RGBA))
{
	RegisterInput("ShadowMap0", _shadowMap);
	RegisterOutput("Target", _target);
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
	ImGui::Begin("Shadow maps in Geometry");
	 //Needs own shader to render texture cube
	//ImGui::Image((void*)(&_testCube->ShaderView()), {100.0f, 100.0f});
	ImGui::End();
}

void GeometryPass::Execute()
{
	auto& common = SceneCommon();

	// Setup renderer
	Renderer::SetRenderState(RenderState::Default);
	Renderer::SetViewportSize(_target->Width(), _target->Height());

	// Update point light data
	PointLightCBuffer data;
	data.PointLights[0].LightTransform = common.PointLight.LightTransform.Transpose();
	data.PointLights[0].Position = common.PointLight.Position;
	data.nPointLights = 1;
	_pointLightCBuffer->Update(data);

	// Render to texture
	_target->Bind();
	_target->Clear();
	_shadowMapTexture->Bind();

	for (auto& drawCommand : common.DrawCommands.at(RenderChannel_Geometry))
	{
		drawCommand.Mesh->Bind();
		_pointLightCBuffer->Bind();
		_shadowSampler->Bind();

		Renderer::Submit(
			[this, drawCommand](const RendererPackage& package)
			{
				_mvpCBuffer->Bind();

				const auto& common = SceneCommon();
				const auto viewProj = common.CameraData.View * common.CameraData.Projection;

				for (const auto& submesh : drawCommand.Mesh->SubMeshes())
				{
					const auto model = submesh.Transform * drawCommand.Mesh->Transform() * drawCommand.Transform;

					_mvpCBuffer->Update({model, model * common.CameraData.View, viewProj, model * viewProj});
					_mvpCBuffer->UploadData();
					package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
				}
			}
		);
	}

	_shadowMapTexture->Unbind();
	_target->Unbind();
}


void GeometryPass::SetViewportSize(uint width, uint height)
{
	_target->Resize(width, height);
}
}

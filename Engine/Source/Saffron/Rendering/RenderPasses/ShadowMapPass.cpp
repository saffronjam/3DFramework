#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/ShadowMapPass.h"

#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
ShadowMapPass::ShadowMapPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon),
	_shadowMap(Framebuffer::Create({Width, Height, {ImageFormat::RGBA, ImageFormat::Depth}})),
	_shadowShader(Shader::Create("Shadow")),
	_depthTextureShader(Shader::Create("DepthTexture")),
	_mvpCBuffer(MvpCBuffer::Create())
{
	RegisterOutput("Target0", _shadowMap);
}

void ShadowMapPass::OnSetupFinished()
{
}

void ShadowMapPass::OnUi()
{
	ImGui::Begin("Shadow map");
	ImGui::Checkbox("Orthographic", &_orthographic);
	Ui::Image(_shadowMap->DepthImage(), *_depthTextureShader, {500.0f, 500.0f});
	ImGui::End();
}

void ShadowMapPass::Execute()
{
	_shadowMap->Bind();
	_shadowMap->Clear();

	for (auto& drawCommand : SceneCommon().DrawCommands.at(RenderChannel_Shadow))
	{
		drawCommand.Mesh->Bind();

		// Light transform
		auto& common = SceneCommon();

		Matrix view, proj;

		if (_orthographic)
		{
			view = Matrix::CreateLookAt(
				Vector3(0.0f, 4.0f, 0.0f),
				Vector3(0.0f, 0.0f, 0.0f),
				Vector3(1.0f, 0.0f, 0.0f)
			);
			proj = Matrix::CreateOrthographicOffCenter(-10.0f, 10.0f, -10.0f, 10.0f, -10.0, 30.0f);
		}
		else
		{
			view = Matrix::CreateLookAt(
				common.PointLight.Position,
				common.PointLight.Position + Vector3{0.0f, -1.0f, 0.0f},
				Vector3{1.0f, 0.0f, 0.0f}
			);
			proj = Matrix::CreatePerspectiveFieldOfView(Math::Pi / 2.0f, 1.0f, 0.1f, 30.0f);
		}

		Matrix viewProj = view * proj;
		common.PointLight.LightTransform = viewProj;

		// Overwrite shader
		_shadowShader->Bind();

		Renderer::SetViewportSize(Width, Height);

		Renderer::Submit(
			[this, drawCommand, view, viewProj](const RendererPackage& package) mutable
			{
				_mvpCBuffer->Bind();

				for (const auto& submesh : drawCommand.Mesh->SubMeshes())
				{
					const auto model = submesh.Transform * drawCommand.Mesh->Transform() * drawCommand.Transform;

					_mvpCBuffer->Update({model, model * view, viewProj, model * viewProj});
					_mvpCBuffer->UploadData();
					package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
				}
			}
		);
	}

	_shadowMap->Unbind();
}
}

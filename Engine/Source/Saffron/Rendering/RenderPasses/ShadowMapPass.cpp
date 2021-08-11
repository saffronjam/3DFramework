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
	RegisterOutput("shadowMap", _shadowMap);
}

void ShadowMapPass::OnUi()
{
	ImGui::Begin("Shadow map");
	Ui::Image(_shadowMap->DepthImage(), *_depthTextureShader, {500.0f, 500.0f});
	ImGui::End();
}

void ShadowMapPass::Execute()
{
	_shadowMap->Bind();
	_shadowMap->Clear();

	for (auto& mesh : SceneCommon().DrawCommands.at(RenderChannel_Shadow))
	{
		mesh->Bind();

		// Overwrite shader
		_shadowShader->Bind();

		Renderer::SetViewportSize(Width, Height);

		Renderer::Submit(
			[this, mesh](const RendererPackage& package)
			{
				_mvpCBuffer->Bind();

				const auto& common = SceneCommon();
				const auto view = Matrix::CreateLookAt(
					common.PointLight.Position,
					common.PointLight.Position + Vector3{0.0f, -1.0f, 0.0f},
					Vector3{1.0f, 0.0f, 0.0f}
				);
				const auto proj = Matrix::CreatePerspectiveFieldOfView(90.0f, 1.0f, 0.1f, 10.0f);
				const auto viewProj = view * proj;

				for (const auto& submesh : mesh->SubMeshes())
				{
					const auto model = submesh.Transform * mesh->Transform();

					_mvpCBuffer->Update({model, model * common.CameraData.View, viewProj, model * viewProj});
					_mvpCBuffer->UploadData();
					package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
				}
			}
		);
	}
}
}

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
	_mvpCBuffer(MvpCBuffer::Create())
{
	RegisterOutput("target", _target);
}

void GeometryPass::Execute()
{
	_target->Bind();
	_target->Clear();

	Renderer::SetViewportSize(_target->Width(), _target->Height());

	for (auto& mesh : SceneCommon().DrawCommands.at(RenderChannel_Geometry))
	{
		mesh->Bind();
		Renderer::Submit(
			[this, mesh](const RendererPackage& package)
			{
				_mvpCBuffer->Bind();

				const auto& common = SceneCommon();
				const auto viewProj = common.CameraData.View * common.CameraData.Projection;

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


void GeometryPass::SetViewportSize(uint width, uint height)
{
	_target->Resize(width, height);
}
}

#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/LinePass.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
LinePass::LinePass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon),
	_shader(Shader::Create("Line")),
	_inputLayout(InputLayout::Create(PosColVertex::Layout(), _shader)),
	_mvpCBuffer(TransformCBuffer::Create())
{
	_vertexBuffer = VertexBuffer::Create(PosColVertex::Layout(), SceneCommon().MaxLines);
	_indexBuffer = IndexBuffer::Create(SceneCommon().MaxLines);

	RegisterInput("Target", _target);
	RegisterOutput("Target", _target);
}

void LinePass::Execute()
{
	const auto& common = SceneCommon();

	// If no lines are submitted, skip pass
	if (common.LinesVertices.empty()) return;

	// Setup renderer
	Renderer::SetRenderState(_renderState);
	Renderer::SetViewportSize(_target->Width(), _target->Height());

	// Copy new vertices into vertex buffer
	_vertexBuffer->SetVertices(common.LinesVertices.data(), common.LinesVertices.size());
	_indexBuffer->SetIndices(common.LineIndices.data(), common.LineIndices.size());

	_target->Bind();
	_shader->Bind();
	_vertexBuffer->Bind();
	_indexBuffer->Bind();
	_inputLayout->Bind();

	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			const auto& common = SceneCommon();
			const auto& view = common.CameraData.View;
			const auto& proj = common.CameraData.Projection;

			_mvpCBuffer->Update({Matrix::Identity, view, view * proj, view * proj});
			_mvpCBuffer->Bind();

			package.Context.DrawIndexed(SceneCommon().LineIndices.size(), 0, 0);
		}
	);

	_target->Unbind();
}
}

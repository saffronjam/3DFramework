#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Rendering/Passes/CompositePass.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
CompositePass::CompositePass(String name) :
	RenderPass(Move(name)),
	_shader(Shader::Create("SceneComposite"))
{
	FramebufferSpecification specification;
	specification.Attachments = {FramebufferTextureFormat::RGBA8};
	specification.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	_outputFramebuffer = Framebuffer::Create(specification);
	
	AddInput(CreateUnique<Stream::Input>("buffer", _finalFramebuffer));
	AddOutput(CreateUnique<Stream::Output>("buffer", _outputFramebuffer));
}

void CompositePass::Execute()
{
	auto& common = SceneRenderer::GetCommon();
	auto& sceneInfo = SceneRenderer::GetSceneInfo();

	auto& geoBuffer = *_finalFramebuffer;
	auto& compBuffer = *_outputFramebuffer;

	Renderer::Begin(_outputFramebuffer);
	
	_shader->Bind();
	_shader->SetFloat("u_Exposure", sceneInfo.SceneCamera.Camera.GetExposure());
	_shader->SetInt("u_TextureSamples", geoBuffer.GetSpecification().Samples);
	_shader->SetFloat2("u_ViewportSize", Vector2(compBuffer.GetWidth(), compBuffer.GetHeight()));
	_shader->SetFloat2("u_FocusPoint", common.FocusPoint);
	_shader->SetInt("u_TextureSamples", geoBuffer.GetSpecification().Samples);
	_shader->SetFloat("u_BloomThreshold", common.BloomThreshold);
	geoBuffer.BindTexture();
	Renderer::Submit([this]()
	{
		glBindTextureUnit(1, _finalFramebuffer->GetDepthAttachmentRendererID());
	});
	Renderer::SubmitFullscreenQuad(nullptr);

	Renderer::End();
}

void CompositePass::OnViewportResize(uint width, uint height)
{
	_outputFramebuffer->Resize(width, height);
}
}

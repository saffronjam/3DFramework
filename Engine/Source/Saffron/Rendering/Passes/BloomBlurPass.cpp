#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Rendering/Passes/BloomBlurPass.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
BloomBlurPass::BloomBlurPass(String name, int iterations) :
	RenderPass(Move(name)),
	_iterations(iterations),
	_bloomBlurShader(Shader::Create("BloomBlur")),
	_bloomBlendShader(Shader::Create("BloomBlend"))
{
	FramebufferSpecification bloomBlurFramebufferSpec;
	bloomBlurFramebufferSpec.Attachments = {FramebufferTextureFormat::RGBA16F};
	bloomBlurFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	_verticalFb = Framebuffer::Create(bloomBlurFramebufferSpec);
	_horiontalFb = Framebuffer::Create(bloomBlurFramebufferSpec);

	FramebufferSpecification bloomBlendFramebufferSpec;
	bloomBlendFramebufferSpec.Attachments = {FramebufferTextureFormat::RGBA8};
	bloomBlendFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	_outputFb = Framebuffer::Create(bloomBlendFramebufferSpec);

	AddOutput(CreateUnique<Stream::Output>("buffer", _outputFb));
}

void BloomBlurPass::Execute()
{
	auto& common = SceneRenderer::GetCommon();
	auto& sceneInfo = SceneRenderer::GetSceneInfo();

	int index = 0;

	int horizontalCounter = 0, verticalCounter = 0;
	for (int i = 0; i < _iterations; i++)
	{
		index = i % 2;
		Renderer::Begin(index == 0 ? _verticalFb : _horiontalFb);
		_bloomBlurShader->Bind();
		_bloomBlurShader->SetBool("u_Horizontal", index);
		if (index) horizontalCounter++;
		else verticalCounter++;
		if (i > 0)
		{
			HorizOrVert(index)->BindTexture();
		}
		else
		{
			auto id = _inputFb->GetColorAttachmentRendererID(1);
			Renderer::Submit([id]()
			{
				glBindTextureUnit(0, id);
			});
		}
		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::End();
	}

	// Composite bloom
	{
		Renderer::Begin(_outputFb);
		_bloomBlendShader->Bind();
		_bloomBlendShader->SetFloat("u_Exposure", sceneInfo.SceneCamera.Camera.GetExposure());
		_bloomBlendShader->SetBool("u_EnableBloom", common.EnableBloom);

		_outputFb->BindTexture(0);
		HorizOrVert(index)->BindTexture(1);

		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::End();
	}
}

void BloomBlurPass::OnViewportResize(Uint32 width, Uint32 height)
{
	_verticalFb->Resize(width, height);
	_horiontalFb->Resize(width, height);
	_outputFb->Resize(width, height);
}

const Shared<Se::Framebuffer>& BloomBlurPass::HorizOrVert(int index)
{
	return (index == 0 ? _verticalFb : _horiontalFb);
}
}

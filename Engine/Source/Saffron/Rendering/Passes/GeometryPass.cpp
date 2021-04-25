#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Rendering/Passes/GeometryPass.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
GeometryPass::GeometryPass(String name) :
	RenderPass(Move(name)),
	_brdflut(Texture2D::Create("BRDF_LUT.tga"))
{
	FramebufferSpecification specification;
	specification.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth };
	specification.Samples = 8;
	specification.ClearColor = { 1.0f, 0.1f, 0.1f, 1.0f };
	_framebuffer = Framebuffer::Create(specification);

	AddInput(CreateUnique<Stream::Input>("shadowMap0", _shadowMaps[0]));
	AddInput(CreateUnique<Stream::Input>("shadowMap1", _shadowMaps[1]));
	AddInput(CreateUnique<Stream::Input>("shadowMap2", _shadowMaps[2]));
	AddInput(CreateUnique<Stream::Input>("shadowMap3", _shadowMaps[3]));
	AddOutput(CreateUnique<Stream::Output>("buffer", _framebuffer));

	Renderer::Submit([this]()
	{
		glGenSamplers(1, &_shadowMapSampler);

		// Setup the shadowmap depth sampler
		glSamplerParameteri(_shadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_shadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_shadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_shadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	});
}

void GeometryPass::Execute()
{
	auto& common = SceneRenderer::GetCommon();
	auto& mainMeshes = SceneRenderer::GetDrawCommands(RenderChannel::Main);
	auto& selectedMeshes = SceneRenderer::GetDrawCommands(RenderChannel::Outline);
	auto& sceneInfo = SceneRenderer::GetSceneInfo();
	auto& options = SceneRenderer::GetOptions();
	auto& smData = SceneRenderer::GetShadowMapData();

	const bool outline = !selectedMeshes.empty();

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		});
	}
	
	Renderer::Begin(_framebuffer);

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilMask(0);
		});
	}

	auto& sceneCamera = sceneInfo.SceneCamera;

	const auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;
	const Vector3 cameraPosition = inverse(sceneInfo.SceneCamera.ViewMatrix)[3]; // TODO: Negate instead

	// Skybox
	const auto& skyboxShader = sceneInfo.SkyboxMaterial->GetShader();
	sceneInfo.SkyboxMaterial->Set("u_InverseVP", inverse(viewProjection));
	sceneInfo.SkyboxMaterial->Set("u_SkyIntensity", sceneInfo.SceneEnvironmentIntensity);
	Renderer::SubmitFullscreenQuad(sceneInfo.SkyboxMaterial);

	const float aspectRatio = static_cast<float>(_framebuffer->GetWidth()) / static_cast<float>(_framebuffer->
		GetHeight());
	float frustumSize = 2.0f * sceneCamera.Near * glm::tan(sceneCamera.Fov * 0.5f) * aspectRatio;

	// Render entities
	for (auto& dc : mainMeshes)
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", sceneCamera.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_LightMatrixCascade0", smData.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", smData.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", smData.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", smData.LightMatrices[3]);
		baseMaterial->Set("u_ShowCascades", smData.ShowCascades);
		baseMaterial->Set("u_LightView", smData.LightViewMatrix);
		baseMaterial->Set("u_CascadeSplits", smData.CascadeSplits);
		baseMaterial->Set("u_SoftShadows", smData.SoftShadows);
		baseMaterial->Set("u_LightSize", smData.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", smData.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", smData.ShadowFade);
		baseMaterial->Set("u_CascadeFading", smData.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", smData.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", sceneInfo.SceneEnvironmentIntensity);

		// SceneEnvironment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", sceneInfo.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", sceneInfo.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", _brdflut);

		// Set lights (TODO: move to light environment and don't do per mesh)
		auto directionalLight = sceneInfo.LightEnvironment.DirectionalLights[0];
		baseMaterial->Set("u_DirectionalLights", directionalLight);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if (rd)
		{
			auto reg = rd->GetRegister();

			auto tex = _shadowMaps[0]->GetDepthAttachmentRendererID();
			auto tex1 = _shadowMaps[1]->GetDepthAttachmentRendererID();
			auto tex2 = _shadowMaps[2]->GetDepthAttachmentRendererID();
			auto tex3 = _shadowMaps[3]->GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3, this]() mutable
			{
				// 4 cascades
				glBindTextureUnit(reg, tex);
				glBindSampler(reg++, _shadowMapSampler);

				glBindTextureUnit(reg, tex1);
				glBindSampler(reg++, _shadowMapSampler);

				glBindTextureUnit(reg, tex2);
				glBindSampler(reg++, _shadowMapSampler);

				glBindTextureUnit(reg, tex3);
				glBindSampler(reg++, _shadowMapSampler);
			});
		}


		auto overrideMaterial = nullptr; // dc.Material;
		Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
	}

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilFunc(GL_ALWAYS, 1, 0xff);
			glStencilMask(0xff);
		});
	}
	for (auto& dc : selectedMeshes)
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", sceneCamera.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_CascadeSplits", smData.CascadeSplits);
		baseMaterial->Set("u_ShowCascades", smData.ShowCascades);
		baseMaterial->Set("u_SoftShadows", smData.SoftShadows);
		baseMaterial->Set("u_LightSize", smData.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", smData.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", smData.ShadowFade);
		baseMaterial->Set("u_CascadeFading", smData.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", smData.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", sceneInfo.SceneEnvironmentIntensity);

		// SceneEnvironment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", sceneInfo.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", sceneInfo.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", _brdflut);

		baseMaterial->Set("u_LightMatrixCascade0", smData.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", smData.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", smData.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", smData.LightMatrices[3]);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("u_DirectionalLights", sceneInfo.LightEnvironment.DirectionalLights[0]);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if (rd)
		{
			auto reg = rd->GetRegister();

			auto tex = _shadowMaps[0]->GetDepthAttachmentRendererID();
			auto tex1 = _shadowMaps[1]->GetDepthAttachmentRendererID();
			auto tex2 = _shadowMaps[2]->GetDepthAttachmentRendererID();
			auto tex3 = _shadowMaps[3]->GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3, this]() mutable
			{
				// 4 cascades
				glBindTextureUnit(reg, tex);
				glBindSampler(reg++, _shadowMapSampler);

				glBindTextureUnit(reg, tex1);
				glBindSampler(reg++, _shadowMapSampler);

				glBindTextureUnit(reg, tex2);
				glBindSampler(reg++, _shadowMapSampler);

				glBindTextureUnit(reg, tex3);
				glBindSampler(reg++, _shadowMapSampler);
			});
		}

		auto overrideMaterial = nullptr; // dc.Material;
		Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
	}

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilMask(0);

			glLineWidth(10);
			glEnable(GL_LINE_SMOOTH);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_DEPTH_TEST);
		});

		// Draw outline here
		common.OutlineMaterial->Set("u_ViewProjection", viewProjection);
		for (auto& dc : selectedMeshes)
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, common.OutlineMaterial);
		}

		Renderer::Submit([]()
		{
			glPointSize(10);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		});
		for (auto& dc : selectedMeshes)
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, common.OutlineMaterial);
		}

		Renderer::Submit([]()
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glStencilMask(0xff);
			glStencilFunc(GL_ALWAYS, 1, 0xff);
			glEnable(GL_DEPTH_TEST);
		});
	}

	// Grid
	if (options.ShowGrid)
	{
		common.GridMaterial->Set("u_ViewProjection", viewProjection);
		Renderer::SubmitQuad(common.GridMaterial,
		                     rotate(Matrix4(1.0f), glm::radians(90.0f), Vector3(1.0f, 0.0f, 0.0f)) * scale(
			                     Matrix4(1.0f), Vector3(16.0f)));
	}

	if (options.ShowBoundingBoxes)
	{
		//Renderer2D::BeginScene(viewProjection);
		//for (auto& dc : instData.DrawList) Renderer::DrawAABB(dc.Mesh, dc.Transform);
		//Renderer2D::EndScene();
	}

	Renderer::End();
}

void GeometryPass::OnViewportResize(uint width, uint height)
{
	_framebuffer->Resize(width, height);
}
}

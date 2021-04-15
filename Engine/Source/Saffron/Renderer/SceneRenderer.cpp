#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneEnvironment.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/SceneRenderer.h"


namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Scene Renderer Data
///////////////////////////////////////////////////////////////////////////

struct SceneRendererData
{
	const Scene* ActiveScene = nullptr;

	struct SceneInfo
	{
		// Resources
		Shared<MaterialInstance> SkyboxMaterial;
		Shared<SceneEnvironment> SceneEnvironment;
		Scene::LightEnvironment SceneLightEnvironment;
		Scene::Light ActiveLight;
	} SceneData;

	Shared<Texture2D> BRDFLUT;
	Shared<Shader> CompositeShader;
	Shared<Shader> BloomBlurShader;
	Shared<Shader> BloomBlendShader;

	Shared<Shader> ShadowMapShader;

	float ShadowMapSize = 20.0f;
	float LightDistance = 0.1f;
	Matrix4f LightMatrices[4];
	Matrix4f LightViewMatrix;
	float CascadeSplitLambda = 0.91f;
	Vector4f CascadeSplits;
	float CascadeFarPlaneOffset = 15.0f, CascadeNearPlaneOffset = -15.0f;
	bool ShowCascades = false;
	bool SoftShadows = true;
	float LightSize = 0.5f;
	float MaxShadowDistance = 200.0f;
	float ShadowFade = 25.0f;
	float CascadeTransitionFade = 1.0f;
	bool CascadeFading = true;

	bool EnableBloom = false;
	float BloomThreshold = 1.5f;

	Vector2f FocusPoint = {0.5f, 0.5f};

	RendererID ShadowMapSampler;

	Shared<SceneRenderer::Target> MainRenderTarget;
	ArrayList<Shared<SceneRenderer::Target>> RenderTargets;
	ArrayList<Shared<SceneRenderer::Target>> RegisteredRenderTargets;

	struct DrawCommand
	{
		Shared<Mesh> Mesh;
		Shared<MaterialInstance> Material;
		Matrix4f Transform;
	};

	ArrayList<DrawCommand> DrawList;
	ArrayList<DrawCommand> SelectedMeshDrawList;
	ArrayList<DrawCommand> ShadowPassDrawList;

	// Grid
	Shared<MaterialInstance> GridMaterial;
	Shared<MaterialInstance> OutlineMaterial;

	SceneRenderer::Options Options;

	// Lines
	static constexpr Uint32 MaxLines = 10000;
	static constexpr Uint32 MaxLineVertices = MaxLines * 2;
	static constexpr Uint32 MaxLineIndices = MaxLines * 2;

	Uint32 LineIndexCount = 0;
	LineVertex* LineVertexBufferBase = nullptr;
	LineVertex* LineVertexBufferPtr = nullptr;

	Shared<Shader> LineShader;
};

static Shared<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

///////////////////////////////////////////////////////////////////////////
/// Scene Renderer Target
///////////////////////////////////////////////////////////////////////////

SceneRenderer::Target::Target(String friendlyName) :
	m_FriendlyName(Move(friendlyName))
{
}

SceneRenderer::Target::~Target()
{
	//const auto removeIterator = std::find(instData.RegisteredRenderTargets.begin(), instData.RegisteredRenderTargets.end(), this);
	//SE_CORE_ASSERT(removeIterator != instData.RegisteredRenderTargets.end(), "Target was not registered");
	//instData.RegisteredRenderTargets.erase(removeIterator);
}

void SceneRenderer::Target::GeometryPass()
{
	auto& instData = *Instance()._data;

	if (!m_CameraData.Camera)
	{
		return;
	}

	const bool outline = !instData.SelectedMeshDrawList.empty();

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		});
	}

	Renderer::BeginRenderPass(m_GeoPass);

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilMask(0);
		});
	}
	const auto viewProjection = m_CameraData.Camera->GetProjectionMatrix() * m_CameraData.ViewMatrix;
	const Vector3f cameraPosition = glm::inverse(m_CameraData.ViewMatrix)[3];

	// Skybox
	auto skyboxShader = instData.SceneData.SkyboxMaterial->GetShader();
	instData.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
	instData.SceneData.SkyboxMaterial->Set("u_SkyIntensity", instData.SceneData.SceneEnvironment->GetIntensity());
	Renderer::SubmitQuad(instData.SceneData.SkyboxMaterial);

	const auto geoSpecWidth = static_cast<float>(m_GeoPass->GetSpecification().TargetFramebuffer->GetWidth());
	const auto geoSpecHeight = static_cast<float>(m_GeoPass->GetSpecification().TargetFramebuffer->GetHeight());

	const float aspectRatio = geoSpecWidth / geoSpecHeight;
	float frustumSize = 2.0f * m_CameraData.Near * glm::tan(m_CameraData.FOV * 0.5f) * aspectRatio;

	// Render entities
	for (auto& dc : instData.DrawList)
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", m_CameraData.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_LightMatrixCascade0", instData.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", instData.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", instData.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", instData.LightMatrices[3]);
		baseMaterial->Set("u_ShowCascades", instData.ShowCascades);
		baseMaterial->Set("u_LightView", instData.LightViewMatrix);
		baseMaterial->Set("u_CascadeSplits", instData.CascadeSplits);
		baseMaterial->Set("u_SoftShadows", instData.SoftShadows);
		baseMaterial->Set("u_LightSize", instData.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", instData.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", instData.ShadowFade);
		baseMaterial->Set("u_CascadeFading", instData.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", instData.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", instData.SceneData.SceneEnvironment->GetIntensity());

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", instData.SceneData.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", instData.SceneData.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", instData.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		auto directionalLight = instData.SceneData.SceneLightEnvironment.DirectionalLights[0];
		baseMaterial->Set("u_DirectionalLights", directionalLight);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if (rd)
		{
			auto reg = rd->GetRegister();

			auto tex = m_ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex1 = m_ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex2 = m_ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex3 = m_ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3, instData]() mutable
			{
				// 4 cascades
				glBindTextureUnit(reg, tex);
				glBindSampler(reg++, instData.ShadowMapSampler);

				glBindTextureUnit(reg, tex1);
				glBindSampler(reg++, instData.ShadowMapSampler);

				glBindTextureUnit(reg, tex2);
				glBindSampler(reg++, instData.ShadowMapSampler);

				glBindTextureUnit(reg, tex3);
				glBindSampler(reg++, instData.ShadowMapSampler);
			});
		}


		const Shared<MaterialInstance> overrideMaterial = nullptr; // dc.Material;
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
	for (auto& dc : instData.SelectedMeshDrawList)
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", m_CameraData.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_CascadeSplits", instData.CascadeSplits);
		baseMaterial->Set("u_ShowCascades", instData.ShowCascades);
		baseMaterial->Set("u_SoftShadows", instData.SoftShadows);
		baseMaterial->Set("u_LightSize", instData.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", instData.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", instData.ShadowFade);
		baseMaterial->Set("u_CascadeFading", instData.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", instData.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", instData.SceneData.SceneEnvironment->GetIntensity());

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", instData.SceneData.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", instData.SceneData.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", instData.BRDFLUT);

		baseMaterial->Set("u_LightMatrixCascade0", instData.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", instData.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", instData.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", instData.LightMatrices[3]);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("u_DirectionalLights", instData.SceneData.SceneLightEnvironment.DirectionalLights[0]);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if (rd)
		{
			auto reg = rd->GetRegister();

			auto tex = m_ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex1 = m_ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex2 = m_ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex3 = m_ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3, instData]() mutable
			{
				// 4 cascades
				glBindTextureUnit(reg, tex);
				glBindSampler(reg++, instData.ShadowMapSampler);

				glBindTextureUnit(reg, tex1);
				glBindSampler(reg++, instData.ShadowMapSampler);

				glBindTextureUnit(reg, tex2);
				glBindSampler(reg++, instData.ShadowMapSampler);

				glBindTextureUnit(reg, tex3);
				glBindSampler(reg++, instData.ShadowMapSampler);
			});
		}

		const Shared<MaterialInstance> overrideMaterial = nullptr; // dc.Material;
		Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
	}

	if (outline)
	{
		Renderer::SetLineThickness(10.0f);
		Renderer::Submit([]()
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilMask(0);

			glEnable(GL_LINE_SMOOTH);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_DEPTH_TEST);
		});

		// Draw outline here
		instData.OutlineMaterial->Set("u_ViewProjection", viewProjection);
		for (auto& dc : instData.SelectedMeshDrawList)
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, instData.OutlineMaterial);
		}

		Renderer::Submit([]()
		{
			glPointSize(10);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		});
		for (auto& dc : instData.SelectedMeshDrawList)
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, instData.OutlineMaterial);
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
	if (GetOptions().ShowGrid)
	{
		instData.GridMaterial->Set("u_ViewProjection", viewProjection);
		Renderer::SubmitQuad(instData.GridMaterial,
		                     glm::rotate(Matrix4f(1.0f), glm::radians(90.0f), Vector3f(1.0f, 0.0f, 0.0f)) * glm::scale(
			                     Matrix4f(1.0f), Vector3f(16.0f)));
	}
	if (GetOptions().ShowMeshBoundingBoxes)
	{
		for (auto& dc : instData.DrawList)
		{
			SubmitAABB(dc.Mesh, dc.Transform);
		}
		for (auto& dc : instData.SelectedMeshDrawList)
		{
			SubmitAABB(dc.Mesh, dc.Transform);
		}
	}

	instData.LineShader->SetMat4("u_ViewProjection", viewProjection);
	Renderer::SubmitLines(instData.LineVertexBufferBase, instData.LineIndexCount, instData.LineShader);

	Renderer::EndRenderPass();
}

void SceneRenderer::Target::CompositePass()
{
	auto& instData = *Instance()._data;

	if (!m_CameraData.Camera)
	{
		return;
	}
	const auto& compositeBuffer = m_CompositePass->GetSpecification().TargetFramebuffer;

	Renderer::BeginRenderPass(m_CompositePass);
	instData.CompositeShader->Bind();
	instData.CompositeShader->SetFloat("u_Exposure", GetCameraData().Camera->GetExposure());
	instData.CompositeShader->SetInt("u_TextureSamples",
	                                 m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
	instData.CompositeShader->SetFloat2("u_ViewportSize",
	                                    Vector2f(compositeBuffer->GetWidth(), compositeBuffer->GetHeight()));
	instData.CompositeShader->SetFloat2("u_FocusPoint", instData.FocusPoint);
	instData.CompositeShader->SetInt("u_TextureSamples",
	                                 m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
	instData.CompositeShader->SetFloat("u_BloomThreshold", instData.BloomThreshold);
	m_GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
	Renderer::Submit([this]()
	{
		glBindTextureUnit(1, m_GeoPass->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID());
	});
	Renderer::SubmitQuad(nullptr);
	Renderer::EndRenderPass();
}

void SceneRenderer::Target::BloomBlurPass()
{
	auto& instData = *Instance()._data;

	const int amount = 10;
	int index = 0;

	int horizontalCounter = 0, verticalCounter = 0;
	for (int i = 0; i < amount; i++)
	{
		index = i % 2;
		Renderer::BeginRenderPass(m_BloomBlurPass[index]);
		instData.BloomBlurShader->Bind();
		instData.BloomBlurShader->SetBool("u_Horizontal", index);
		if (index) horizontalCounter++;
		else verticalCounter++;
		if (i > 0)
		{
			auto fb = m_BloomBlurPass[1 - index]->GetSpecification().TargetFramebuffer;
			fb->BindTexture();
		}
		else
		{
			auto fb = m_CompositePass->GetSpecification().TargetFramebuffer;
			auto id = fb->GetColorAttachmentRendererID(1);
			Renderer::Submit([id]()
			{
				glBindTextureUnit(0, id);
			});
		}
		Renderer::SubmitQuad(nullptr);
		Renderer::EndRenderPass();
	}

	// Composite bloom
	{
		Renderer::BeginRenderPass(m_BloomBlendPass);
		instData.BloomBlendShader->Bind();
		instData.BloomBlendShader->SetFloat("u_Exposure", m_CameraData.Camera->GetExposure());
		instData.BloomBlendShader->SetBool("u_EnableBloom", instData.EnableBloom);

		m_CompositePass->GetSpecification().TargetFramebuffer->BindTexture(0);
		m_BloomBlurPass[index]->GetSpecification().TargetFramebuffer->BindTexture(1);

		Renderer::SubmitQuad(nullptr);
		Renderer::EndRenderPass();
	}
}

void SceneRenderer::Target::ShadowMapPass()
{
	auto& instData = *Instance()._data;

	auto& directionalLights = instData.SceneData.SceneLightEnvironment.DirectionalLights;
	if (directionalLights[0].Multiplier == 0.0f || !directionalLights[0].CastShadows)
	{
		for (auto& m_ShadowMapRenderPas : m_ShadowMapRenderPass)
		{
			// Clear shadow maps
			Renderer::BeginRenderPass(m_ShadowMapRenderPas);
			Renderer::EndRenderPass();
		}
		return;
	}

	CascadeData cascades[4];
	CalculateCascades(cascades, directionalLights[0].Direction);
	instData.LightViewMatrix = cascades[0].View;

	Renderer::Submit([]()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	});

	for (int i = 0; i < 4; i++)
	{
		instData.CascadeSplits[i] = cascades[i].SplitDepth;

		Renderer::BeginRenderPass(m_ShadowMapRenderPass[i]);

		Matrix4f shadowMapVP = cascades[i].ViewProj;
		instData.ShadowMapShader->SetMat4("u_ViewProjection", shadowMapVP);

		static Matrix4f scaleBiasMatrix = glm::scale(Matrix4f(1.0f), {0.5f, 0.5f, 0.5f}) * glm::translate(
			Matrix4f(1.0f), {1, 1, 1});
		instData.LightMatrices[i] = scaleBiasMatrix * cascades[i].ViewProj;


		// Render entities
		for (auto& dc : instData.ShadowPassDrawList)
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, instData.ShadowMapShader);
		}

		Renderer::EndRenderPass();
	}
}

Vector2u SceneRenderer::Target::GetSize()
{
	SE_CORE_ASSERT(
		m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Width == m_CompositePass->GetSpecification()
		.TargetFramebuffer->GetSpecification().Width && m_GeoPass->GetSpecification().TargetFramebuffer->
		GetSpecification().Height == m_CompositePass->GetSpecification().TargetFramebuffer->GetSpecification().Height);
	return {
		m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Width,
		m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Height
	};
}

void SceneRenderer::Target::SetSize(Uint32 width, Uint32 height)
{
	m_GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
	m_CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
}

void SceneRenderer::Target::ResetRenderingStats()
{
	memset(&m_RenderingStats, 0, sizeof(RenderingStats));
}

const Shared<Texture2D>& SceneRenderer::Target::GetFinalColorBuffer() const
{
	SE_CORE_ASSERT(false, "Not implemented");
	return nullptr;
}

RendererID SceneRenderer::Target::GetFinalColorBufferRendererID() const
{
	return m_CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
}

void SceneRenderer::Target::CalculateCascades(CascadeData* cascades, const Vector3f& lightDirection)
{
	auto& instData = *Instance()._data;

	FrustumBounds frustumBounds[3];

	auto& sceneCamera = m_CameraData;
	auto viewProjection = sceneCamera.Camera->GetProjectionMatrix() * sceneCamera.ViewMatrix;

	const int SHADOW_MAP_CASCADE_COUNT = 4;
	float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];

	// TODO: less hard-coding!
	float nearClip = 0.1f;
	float farClip = 1000.0f;
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	// Calculate split depths based on view camera frustum
	// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = instData.CascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - nearClip) / clipRange;
	}

	cascadeSplits[3] = 0.3f;

	// Manually set cascades here
	// cascadeSplits[0] = 0.05f;
	// cascadeSplits[1] = 0.15f;
	// cascadeSplits[2] = 0.3f;
	// cascadeSplits[3] = 1.0f;

	// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0;
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float splitDist = cascadeSplits[i];

		Vector3f frustumCorners[8] = {
			Vector3f(-1.0f, 1.0f, -1.0f), Vector3f(1.0f, 1.0f, -1.0f), Vector3f(1.0f, -1.0f, -1.0f),
			Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(-1.0f, 1.0f, 1.0f), Vector3f(1.0f, 1.0f, 1.0f),
			Vector3f(1.0f, -1.0f, 1.0f), Vector3f(-1.0f, -1.0f, 1.0f),
		};

		// Project frustum corners into world space
		Matrix4f invCam = glm::inverse(viewProjection);
		for (uint32_t i = 0; i < 8; i++)
		{
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			Vector3f dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + dist * splitDist;
			frustumCorners[i] = frustumCorners[i] + dist * lastSplitDist;
		}

		// Instance frustum center
		Vector3f frustumCenter = Vector3f(0.0f);
		for (uint32_t i = 0; i < 8; i++) frustumCenter += frustumCorners[i];

		frustumCenter /= 8.0f;

		//frustumCenter *= 0.01f;

		float radius = 0.0f;
		for (uint32_t i = 0; i < 8; i++)
		{
			float distance = glm::length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		Vector3f maxExtents = Vector3f(radius);
		Vector3f minExtents = -maxExtents;

		Vector3f lightDir = -lightDirection;
		Matrix4f lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter,
		                                       Vector3f(0.0f, 0.0f, 1.0f));
		Matrix4f lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y,
		                                       0.0f + instData.CascadeNearPlaneOffset,
		                                       maxExtents.z - minExtents.z + instData.CascadeFarPlaneOffset);

		// Store split distance and matrix in cascade
		cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
		cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
		cascades[i].View = lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}
}

Shared<SceneRenderer::Target> SceneRenderer::Target::Create(String friendlyName, Uint32 width, Uint32 height)
{
	auto& instData = *Instance()._data;

	Shared<Target> target = CreateShared<Target>(Move(friendlyName));

	Framebuffer::Specification geoFramebufferSpec;
	geoFramebufferSpec.Width = width;
	geoFramebufferSpec.Height = height;
	geoFramebufferSpec.Attachments = {
		Framebuffer::TextureFormat::RGBA16F, Framebuffer::TextureFormat::RGBA16F, Framebuffer::TextureFormat::Depth
	};
	geoFramebufferSpec.Samples = 8;
	geoFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPass::Specification geoRenderPassSpec;
	geoRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(geoFramebufferSpec);
	target->m_GeoPass = Factory::Create<RenderPass>(geoRenderPassSpec);

	Framebuffer::Specification compFramebufferSpec;
	compFramebufferSpec.Attachments = {Framebuffer::TextureFormat::RGBA8};
	compFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPass::Specification compRenderPassSpec;
	compRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(compFramebufferSpec);
	target->m_CompositePass = Factory::Create<RenderPass>(compRenderPassSpec);

	Framebuffer::Specification bloomBlurFramebufferSpec;
	bloomBlurFramebufferSpec.Attachments = {Framebuffer::TextureFormat::RGBA16F};
	bloomBlurFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPass::Specification bloomBlurRenderPassSpec;
	bloomBlurRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(bloomBlurFramebufferSpec);
	target->m_BloomBlurPass[0] = Factory::Create<RenderPass>(bloomBlurRenderPassSpec);
	bloomBlurRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(bloomBlurFramebufferSpec);
	target->m_BloomBlurPass[1] = Factory::Create<RenderPass>(bloomBlurRenderPassSpec);

	Framebuffer::Specification bloomBlendFramebufferSpec;
	bloomBlendFramebufferSpec.Attachments = {Framebuffer::TextureFormat::RGBA8};
	bloomBlendFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPass::Specification bloomBlendRenderPassSpec;
	bloomBlendRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(bloomBlendFramebufferSpec);
	target->m_BloomBlendPass = Factory::Create<RenderPass>(bloomBlendRenderPassSpec);

	// Shadow Map
	Framebuffer::Specification shadowMapFramebufferSpec;
	shadowMapFramebufferSpec.Width = 4096;
	shadowMapFramebufferSpec.Height = 4096;
	shadowMapFramebufferSpec.Attachments = {Framebuffer::TextureFormat::DEPTH32F};
	shadowMapFramebufferSpec.ClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
	shadowMapFramebufferSpec.NoResize = true;

	// 4 cascades
	for (auto& shadowMapRenderPass : target->m_ShadowMapRenderPass)
	{
		RenderPass::Specification shadowMapRenderPassSpec;
		shadowMapRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(shadowMapFramebufferSpec);
		shadowMapRenderPass = Factory::Create<RenderPass>(shadowMapRenderPassSpec);
	}

	Renderer::Submit([]()
	{
		auto& instData = *Instance()._data;

		glGenSamplers(1, &instData.ShadowMapSampler);

		// Setup the shadowmap depth sampler
		glSamplerParameteri(instData.ShadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(instData.ShadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(instData.ShadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(instData.ShadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	});


	instData.RegisteredRenderTargets.push_back(target);

	target->Enable();

	return target;
}


///////////////////////////////////////////////////////////////////////////
/// Scene Renderer
///////////////////////////////////////////////////////////////////////////


SceneRenderer::SceneRenderer() :
	Instansiated(this),
	_data(CreateUnique<SceneRendererData>())
{
	_data->MainRenderTarget = Target::Create("Main", 1280, 720);

	_data->CompositeShader = Factory::Create<Shader>("SceneComposite");
	_data->BloomBlurShader = Factory::Create<Shader>("BloomBlur");
	_data->BloomBlendShader = Factory::Create<Shader>("BloomBlend");
	_data->BRDFLUT = Factory::Create<Texture2D>("BRDF_LUT.tga");

	// Grid
	const auto gridShader = Factory::Create<Shader>(Filepath{"Grid"});
	_data->GridMaterial = MaterialInstance::Create(Factory::Create<Material>(gridShader));
	_data->GridMaterial->SetFlag(Material::Flag::TwoSided, true);
	const float gridScale = 16.025f, gridSize = 0.025f;
	_data->GridMaterial->Set("u_Scale", gridScale);
	_data->GridMaterial->Set("u_Res", gridSize);

	// Outline
	const auto outlineShader = Factory::Create<Shader>(Filepath{"Outline"});
	_data->OutlineMaterial = MaterialInstance::Create(Factory::Create<Material>(outlineShader));
	_data->OutlineMaterial->SetFlag(Material::Flag::DepthTest, false);

	// Lines
	_data->LineShader = Factory::Create<Shader>(Filepath{"Renderer2D_Line"});
	_data->LineVertexBufferBase = new LineVertex[SceneRendererData::MaxLineVertices];

	// Shadow Map
	_data->ShadowMapShader = Factory::Create<Shader>(Filepath{"ShadowMap"});
}

SceneRenderer::~SceneRenderer()
{
	_instance = nullptr;
}

void SceneRenderer::BeginScene(const Scene* scene, ArrayList<Shared<Target>> targets)
{
	auto& instData = *Instance()._data;

	SE_CORE_ASSERT(!instData.ActiveScene, "Already an active scene");

	instData.ActiveScene = scene;
	instData.RenderTargets = Move(targets);

	instData.SceneData.SkyboxMaterial = scene->GetSkybox().Material;
	instData.SceneData.SceneEnvironment = scene->GetSceneEnvironment();
	instData.SceneData.ActiveLight = scene->GetLight();
	instData.SceneData.SceneLightEnvironment = scene->GetLightEnvironment();
	instData.LineIndexCount = 0;
	instData.LineVertexBufferPtr = instData.LineVertexBufferBase;
}

void SceneRenderer::EndScene()
{
	auto& instData = *Instance()._data;

	SE_CORE_ASSERT(instData.ActiveScene, "No active scene");

	instData.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::OnGuiRender()
{
	auto& instData = *Instance()._data;

	ImGui::Begin("Scene Renderer");

	if (Gui::BeginTreeNode("Shadows"))
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Soft Shadows", instData.SoftShadows);
		Gui::Property("Light Size", instData.LightSize, 0.01f);
		Gui::Property("Max Shadow Distance", instData.MaxShadowDistance, 1.0f);
		Gui::Property("Shadow Fade", instData.ShadowFade, 5.0f);
		Gui::EndPropertyGrid();
		if (Gui::BeginTreeNode("Cascade Settings"))
		{
			Gui::BeginPropertyGrid();
			Gui::Property("Show Cascades", instData.ShowCascades);
			Gui::Property("Cascade Fading", instData.CascadeFading);
			Gui::Property("Cascade Transition Fade", instData.CascadeTransitionFade, 0.05f, 0.0f, FLT_MAX);
			Gui::Property("Cascade Split", instData.CascadeSplitLambda, 0.01f);
			Gui::Property("CascadeNearPlaneOffset", instData.CascadeNearPlaneOffset, 0.1f, -FLT_MAX, 0.0f);
			Gui::Property("CascadeFarPlaneOffset", instData.CascadeFarPlaneOffset, 0.1f, 0.0f, FLT_MAX);
			Gui::EndPropertyGrid();
			Gui::EndTreeNode();
		}
		if (Gui::BeginTreeNode("Shadow Map", false))
		{
			int nodeID = 0;
			for (const auto& target : instData.RegisteredRenderTargets)
			{
				if (Gui::BeginTreeNode("Target: " + target->GetName() + "##" + std::to_string(nodeID++), false))
				{
					static int cascadeIndex = 0;
					const auto framebuffer = target->GetShadowMapRenderPass(cascadeIndex)->GetSpecification().
					                                 TargetFramebuffer;
					const auto rendererID = framebuffer->GetDepthAttachmentRendererID();

					const float regionSize = ImGui::GetContentRegionAvailWidth();
					// (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
					Gui::BeginPropertyGrid();
					Gui::Property("Cascade Index", cascadeIndex, 0, 3, 1, Gui::PropertyFlag::Slider);
					Gui::EndPropertyGrid();
					ImGui::Image(reinterpret_cast<ImTextureID>(rendererID), {regionSize, regionSize}, {0, 1}, {1, 0});
					Gui::EndTreeNode();
				}
			}
			Gui::EndTreeNode();
		}
		Gui::EndTreeNode();
	}
	int nodeID = 0;
	if (Gui::BeginTreeNode("Bloom"))
	{
		for (const auto& target : instData.RegisteredRenderTargets)
		{
			if (Gui::BeginTreeNode("Target: " + target->GetName() + "##" + std::to_string(nodeID++), false))
			{
				Gui::BeginPropertyGrid();
				Gui::Property("Bloom", instData.EnableBloom);
				Gui::Property("Bloom threshold", instData.BloomThreshold, 0.05f);
				Gui::EndPropertyGrid();

				auto framebuffer = target->GetBloomBlurPass(0)->GetSpecification().TargetFramebuffer;
				const auto rendererID = framebuffer->GetColorAttachmentRendererID();

				const float size = ImGui::GetContentRegionAvailWidth();
				// (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
				const float w = size;
				const float h = w / (static_cast<float>(framebuffer->GetWidth()) / static_cast<float>(framebuffer->
					GetHeight()));
				ImGui::Image(reinterpret_cast<ImTextureID>(rendererID), {w, h}, {0, 1}, {1, 0});
				Gui::EndTreeNode();
			}
		}
		Gui::EndTreeNode();
	}

	ImGui::End();
}

void SceneRenderer::SubmitMesh(const Shared<Mesh>& mesh, const Matrix4f& transform,
                               const Shared<MaterialInstance>& overrideMaterial)
{
	auto& instData = *Instance()._data;

	// TODO: Culling, sorting, etc.
	instData.DrawList.push_back({mesh, overrideMaterial, transform});
	instData.ShadowPassDrawList.push_back({mesh, overrideMaterial, transform});
}

void SceneRenderer::SubmitSelectedMesh(const Shared<Mesh>& mesh, const Matrix4f& transform)
{
	auto& instData = *Instance()._data;

	instData.SelectedMeshDrawList.push_back({mesh, nullptr, transform});
	instData.ShadowPassDrawList.push_back({mesh, nullptr, transform});
}

void SceneRenderer::SubmitLine(const Vector3f& first, const Vector3f& second, const Vector4f& color)
{
	auto& instData = *Instance()._data;

	if (instData.LineIndexCount >= SceneRendererData::MaxLineIndices)
	{
		return;
	}

	instData.LineVertexBufferPtr->Position = first;
	instData.LineVertexBufferPtr->Color = color;
	instData.LineVertexBufferPtr++;

	instData.LineVertexBufferPtr->Position = second;
	instData.LineVertexBufferPtr->Color = color;
	instData.LineVertexBufferPtr++;

	instData.LineIndexCount += 2;
}

void SceneRenderer::SubmitAABB(Shared<Mesh> mesh, const Matrix4f& transform, const Vector4f& color)
{
	for (const auto& submesh : mesh->GetSubmeshes())
	{
		const auto& aabb = submesh.BoundingBox;
		auto aabbTransform = transform * mesh->GetLocalTransform() * submesh.Transform;
		SubmitAABB(aabb, aabbTransform);
	}
}

void SceneRenderer::SubmitAABB(const AABB& aabb, const Matrix4f& transform, const Vector4f& color)
{
	Vector4f corners[8] = {
		transform * Vector4f{aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f},
		transform * Vector4f{aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f},
		transform * Vector4f{aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f},
		transform * Vector4f{aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f},
		transform * Vector4f{aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f},
		transform * Vector4f{aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f},
		transform * Vector4f{aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f},
		transform * Vector4f{aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f}
	};

	for (Uint32 i = 0; i < 4; i++) SubmitLine(corners[i], corners[(i + 1) % 4], color);

	for (Uint32 i = 0; i < 4; i++) SubmitLine(corners[i + 4], corners[(i + 1) % 4 + 4], color);

	for (Uint32 i = 0; i < 4; i++) SubmitLine(corners[i], corners[i + 4], color);
}


Shared<SceneEnvironment> SceneRenderer::CreateSceneEnvironment(const Filepath &filepath)
{
	const Uint32 cubemapSize = 2048;
	const Uint32 irradianceMapSize = 32;

	Shared<TextureCube> envUnfiltered = Factory::Create<TextureCube
	>(Texture::Format::Float16, cubemapSize, cubemapSize);
	if (!equirectangularConversionShader)
	{
		equirectangularConversionShader = Factory::Create<Shader>("EquirectangularToCubeMap");
	}
	const Shared<Texture2D> envEquirect = Factory::Create<Texture2D>(filepath);
	SE_CORE_ASSERT(envEquirect->GetFormat() == Texture::Format::Float16, "Texture is not HDR!");

	equirectangularConversionShader->Bind();
	envEquirect->Bind();
	Renderer::Submit([envUnfiltered, cubemapSize, envEquirect]()
	{
		glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
		glGenerateTextureMipmap(envUnfiltered->GetRendererID());
	});

	if (!envFilteringShader)
		envFilteringShader = Factory::Create<Shader>("EnvironmentMipFilter");

	Shared<TextureCube> envFiltered = Factory::Create<TextureCube>(Texture::Format::Float16, cubemapSize, cubemapSize);

	Renderer::Submit([envUnfiltered, envFiltered]()
	{
		glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
		                   envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, envFiltered->GetWidth(),
		                   envFiltered->GetHeight(), 6);
	});

	envFilteringShader->Bind();
	envUnfiltered->Bind();

	Renderer::Submit([envUnfiltered, envFiltered, cubemapSize]()
	{
		const float deltaRoughness = 1.0f / glm::max(static_cast<float>(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
		for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
		{
			const GLuint numGroups = glm::max(1, size / 32);
			glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
			glDispatchCompute(numGroups, numGroups, 6);
		}
	});

	if (!envIrradianceShader)
	{
		envIrradianceShader = Factory::Create<Shader>("EnvironmentIrradiance");
	}

	Shared<TextureCube> irradianceMap = Factory::Create<TextureCube>(Texture::Format::Float16, irradianceMapSize,
	                                                                 irradianceMapSize);
	envIrradianceShader->Bind();
	envFiltered->Bind();
	Renderer::Submit([irradianceMap]()
	{
		glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
		glGenerateTextureMipmap(irradianceMap->GetRendererID());
	});

	return CreateShared<SceneEnvironment>(Move(filepath), envFiltered, irradianceMap);
}


Shared<SceneRenderer::Target>& SceneRenderer::GetMainTarget()
{
	return Instance()._data->MainRenderTarget;
}

SceneRenderer::Options& SceneRenderer::GetOptions()
{
	return Instance()._data->Options;
}

void SceneRenderer::SetFocusPoint(const Vector2f& focusPoint)
{
	Instance()._data->FocusPoint = focusPoint;
}

void SceneRenderer::FlushDrawList()
{
	auto& instData = *Instance()._data;
	SE_CORE_ASSERT(!instData.ActiveScene, "");

	for (auto& target : instData.RenderTargets)
	{
		if (target->IsEnabled())
		{
			target->ResetRenderingStats();
			auto& stats = target->GetRenderingStats();
			{
				Renderer::Submit([&stats]() { stats.ShadowPassTimer.Sync(); });
				target->ShadowMapPass();
				Renderer::Submit([&stats] { stats.ShadowPass = stats.ShadowPassTimer.Mark(); });
			}
			{
				Renderer::Submit([&stats]() { stats.GeometryPassTimer.Sync(); });
				target->GeometryPass();
				Renderer::Submit([&stats] { stats.GeometryPass = stats.GeometryPassTimer.Mark(); });
			}
			{
				Renderer::Submit([&stats]() { stats.CompositePassTimer.Sync(); });
				target->CompositePass();
				Renderer::Submit([&stats] { stats.CompositePass = stats.CompositePassTimer.Mark(); });

				//	BloomBlurPass();
			}
		}
	}

	instData.DrawList.clear();
	instData.SelectedMeshDrawList.clear();
	instData.ShadowPassDrawList.clear();
	instData.SceneData = {};
}
}

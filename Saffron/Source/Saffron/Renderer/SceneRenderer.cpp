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
	const Scene *ActiveScene = nullptr;
	struct SceneInfo
	{
		// Resources
		std::shared_ptr<MaterialInstance> SkyboxMaterial;
		std::shared_ptr<SceneEnvironment> SceneEnvironment;
		Scene::LightEnvironment SceneLightEnvironment;
		Scene::Light ActiveLight;
	} SceneData;

	std::shared_ptr<Texture2D> BRDFLUT;
	std::shared_ptr<Shader> CompositeShader;
	std::shared_ptr<Shader> BloomBlurShader;
	std::shared_ptr<Shader> BloomBlendShader;

	std::shared_ptr<Shader> ShadowMapShader;

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

	Vector2f FocusPoint = { 0.5f, 0.5f };

	RendererID ShadowMapSampler;

	std::shared_ptr<SceneRenderer::Target> MainRenderTarget;
	ArrayList<std::shared_ptr<SceneRenderer::Target>> RenderTargets;
	ArrayList<std::shared_ptr<SceneRenderer::Target>> RegisteredRenderTargets;

	struct DrawCommand
	{
		std::shared_ptr<Mesh> Mesh;
		std::shared_ptr<MaterialInstance> Material;
		Matrix4f Transform;
	};
	ArrayList<DrawCommand> DrawList;
	ArrayList<DrawCommand> SelectedMeshDrawList;
	ArrayList<DrawCommand> ShadowPassDrawList;

	// Grid
	std::shared_ptr<MaterialInstance> GridMaterial;
	std::shared_ptr<MaterialInstance> OutlineMaterial;

	SceneRenderer::Options Options;

	// Lines
	static constexpr Uint32 MaxLines = 10000;
	static constexpr Uint32 MaxLineVertices = MaxLines * 2;
	static constexpr Uint32 MaxLineIndices = MaxLines * 2;

	Uint32 LineIndexCount = 0;
	LineVertex *LineVertexBufferBase = nullptr;
	LineVertex *LineVertexBufferPtr = nullptr;

	std::shared_ptr<Shader> LineShader;
};

static SceneRendererData s_Data;
static std::shared_ptr<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

///////////////////////////////////////////////////////////////////////////
/// Scene Renderer Target
///////////////////////////////////////////////////////////////////////////

SceneRenderer::Target::Target(String friendlyName)
	: m_FriendlyName(Move(friendlyName))
{
}

SceneRenderer::Target::~Target()
{
	//const auto removeIterator = std::find(s_Data.RegisteredRenderTargets.begin(), s_Data.RegisteredRenderTargets.end(), this);
	//SE_CORE_ASSERT(removeIterator != s_Data.RegisteredRenderTargets.end(), "Target was not registered");
	//s_Data.RegisteredRenderTargets.erase(removeIterator);
}

void SceneRenderer::Target::GeometryPass()
{
	if ( !m_CameraData.Camera )
	{
		return;
	}

	const bool outline = !s_Data.SelectedMeshDrawList.empty();

	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
						 });
	}

	Renderer::BeginRenderPass(m_GeoPass);

	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilMask(0);
						 });
	}
	const auto viewProjection = m_CameraData.Camera->GetProjectionMatrix() * m_CameraData.ViewMatrix;
	const Vector3f cameraPosition = glm::inverse(m_CameraData.ViewMatrix)[3];

	// Skybox
	auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
	s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
	s_Data.SceneData.SkyboxMaterial->Set("u_SkyIntensity", s_Data.SceneData.SceneEnvironment->GetIntensity());
	Renderer::SubmitQuad(s_Data.SceneData.SkyboxMaterial);

	const auto geoSpecWidth = static_cast<float>(m_GeoPass->GetSpecification().TargetFramebuffer->GetWidth());
	const auto geoSpecHeight = static_cast<float>(m_GeoPass->GetSpecification().TargetFramebuffer->GetHeight());

	const float aspectRatio = geoSpecWidth / geoSpecHeight;
	float frustumSize = 2.0f * m_CameraData.Near * glm::tan(m_CameraData.FOV * 0.5f) * aspectRatio;

	// Render entities
	for ( auto &dc : s_Data.DrawList )
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", m_CameraData.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_LightMatrixCascade0", s_Data.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", s_Data.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", s_Data.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", s_Data.LightMatrices[3]);
		baseMaterial->Set("u_ShowCascades", s_Data.ShowCascades);
		baseMaterial->Set("u_LightView", s_Data.LightViewMatrix);
		baseMaterial->Set("u_CascadeSplits", s_Data.CascadeSplits);
		baseMaterial->Set("u_SoftShadows", s_Data.SoftShadows);
		baseMaterial->Set("u_LightSize", s_Data.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", s_Data.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", s_Data.ShadowFade);
		baseMaterial->Set("u_CascadeFading", s_Data.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", s_Data.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", s_Data.SceneData.SceneEnvironment->GetIntensity());

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		auto directionalLight = s_Data.SceneData.SceneLightEnvironment.DirectionalLights[0];
		baseMaterial->Set("u_DirectionalLights", directionalLight);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if ( rd )
		{
			auto reg = rd->GetRegister();

			auto tex = m_ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex1 = m_ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex2 = m_ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex3 = m_ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3]() mutable
							 {
								 // 4 cascades
								 glBindTextureUnit(reg, tex);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);

								 glBindTextureUnit(reg, tex1);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);

								 glBindTextureUnit(reg, tex2);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);

								 glBindTextureUnit(reg, tex3);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);
							 });
		}


		const std::shared_ptr<MaterialInstance> overrideMaterial = nullptr; // dc.Material;
		Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
	}

	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilFunc(GL_ALWAYS, 1, 0xff);
							 glStencilMask(0xff);
						 });
	}
	for ( auto &dc : s_Data.SelectedMeshDrawList )
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", m_CameraData.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_CascadeSplits", s_Data.CascadeSplits);
		baseMaterial->Set("u_ShowCascades", s_Data.ShowCascades);
		baseMaterial->Set("u_SoftShadows", s_Data.SoftShadows);
		baseMaterial->Set("u_LightSize", s_Data.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", s_Data.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", s_Data.ShadowFade);
		baseMaterial->Set("u_CascadeFading", s_Data.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", s_Data.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", s_Data.SceneData.SceneEnvironment->GetIntensity());

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		baseMaterial->Set("u_LightMatrixCascade0", s_Data.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", s_Data.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", s_Data.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", s_Data.LightMatrices[3]);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("u_DirectionalLights", s_Data.SceneData.SceneLightEnvironment.DirectionalLights[0]);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if ( rd )
		{
			auto reg = rd->GetRegister();

			auto tex = m_ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex1 = m_ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex2 = m_ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
			auto tex3 = m_ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3]() mutable
							 {
								 // 4 cascades
								 glBindTextureUnit(reg, tex);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);

								 glBindTextureUnit(reg, tex1);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);

								 glBindTextureUnit(reg, tex2);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);

								 glBindTextureUnit(reg, tex3);
								 glBindSampler(reg++, s_Data.ShadowMapSampler);
							 });
		}

		const std::shared_ptr<MaterialInstance> overrideMaterial = nullptr; // dc.Material;
		Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
	}

	if ( outline )
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
		s_Data.OutlineMaterial->Set("u_ViewProjection", viewProjection);
		for ( auto &dc : s_Data.SelectedMeshDrawList )
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
		}

		Renderer::Submit([]()
						 {
							 glPointSize(10);
							 glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
						 });
		for ( auto &dc : s_Data.SelectedMeshDrawList )
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
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
	if ( GetOptions().ShowGrid )
	{
		s_Data.GridMaterial->Set("u_ViewProjection", viewProjection);
		Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(Matrix4f(1.0f), glm::radians(90.0f), Vector3f(1.0f, 0.0f, 0.0f)) * glm::scale(Matrix4f(1.0f), Vector3f(16.0f)));
	}
	if ( GetOptions().ShowMeshBoundingBoxes )
	{
		for ( auto &dc : s_Data.DrawList )
		{
			SubmitAABB(dc.Mesh, dc.Transform);
		}
		for ( auto &dc : s_Data.SelectedMeshDrawList )
		{
			SubmitAABB(dc.Mesh, dc.Transform);
		}
	}

	s_Data.LineShader->SetMat4("u_ViewProjection", viewProjection);
	Renderer::SubmitLines(s_Data.LineVertexBufferBase, s_Data.LineIndexCount, s_Data.LineShader);

	Renderer::EndRenderPass();
}

void SceneRenderer::Target::CompositePass()
{
	if ( !m_CameraData.Camera )
	{
		return;
	}
	const auto &compositeBuffer = m_CompositePass->GetSpecification().TargetFramebuffer;

	Renderer::BeginRenderPass(m_CompositePass);
	s_Data.CompositeShader->Bind();
	s_Data.CompositeShader->SetFloat("u_Exposure", GetCameraData().Camera->GetExposure());
	s_Data.CompositeShader->SetInt("u_TextureSamples", m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
	s_Data.CompositeShader->SetFloat2("u_ViewportSize", Vector2f(compositeBuffer->GetWidth(), compositeBuffer->GetHeight()));
	s_Data.CompositeShader->SetFloat2("u_FocusPoint", s_Data.FocusPoint);
	s_Data.CompositeShader->SetInt("u_TextureSamples", m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
	s_Data.CompositeShader->SetFloat("u_BloomThreshold", s_Data.BloomThreshold);
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
	const int amount = 10;
	int index = 0;

	int horizontalCounter = 0, verticalCounter = 0;
	for ( int i = 0; i < amount; i++ )
	{
		index = i % 2;
		Renderer::BeginRenderPass(m_BloomBlurPass[index]);
		s_Data.BloomBlurShader->Bind();
		s_Data.BloomBlurShader->SetBool("u_Horizontal", index);
		if ( index )
			horizontalCounter++;
		else
			verticalCounter++;
		if ( i > 0 )
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
		s_Data.BloomBlendShader->Bind();
		s_Data.BloomBlendShader->SetFloat("u_Exposure", m_CameraData.Camera->GetExposure());
		s_Data.BloomBlendShader->SetBool("u_EnableBloom", s_Data.EnableBloom);

		m_CompositePass->GetSpecification().TargetFramebuffer->BindTexture(0);
		m_BloomBlurPass[index]->GetSpecification().TargetFramebuffer->BindTexture(1);

		Renderer::SubmitQuad(nullptr);
		Renderer::EndRenderPass();
	}
}

void SceneRenderer::Target::ShadowMapPass()
{
	auto &directionalLights = s_Data.SceneData.SceneLightEnvironment.DirectionalLights;
	if ( directionalLights[0].Multiplier == 0.0f || !directionalLights[0].CastShadows )
	{
		for ( auto &m_ShadowMapRenderPas : m_ShadowMapRenderPass )
		{
			// Clear shadow maps
			Renderer::BeginRenderPass(m_ShadowMapRenderPas);
			Renderer::EndRenderPass();
		}
		return;
	}

	CascadeData cascades[4];
	CalculateCascades(cascades, directionalLights[0].Direction);
	s_Data.LightViewMatrix = cascades[0].View;

	Renderer::Submit([]()
					 {
						 glEnable(GL_CULL_FACE);
						 glCullFace(GL_BACK);
					 });

	for ( int i = 0; i < 4; i++ )
	{
		s_Data.CascadeSplits[i] = cascades[i].SplitDepth;

		Renderer::BeginRenderPass(m_ShadowMapRenderPass[i]);

		Matrix4f shadowMapVP = cascades[i].ViewProj;
		s_Data.ShadowMapShader->SetMat4("u_ViewProjection", shadowMapVP);

		static Matrix4f scaleBiasMatrix = glm::scale(Matrix4f(1.0f), { 0.5f, 0.5f, 0.5f }) * glm::translate(Matrix4f(1.0f), { 1, 1, 1 });
		s_Data.LightMatrices[i] = scaleBiasMatrix * cascades[i].ViewProj;


		// Render entities
		for ( auto &dc : s_Data.ShadowPassDrawList )
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.ShadowMapShader);
		}

		Renderer::EndRenderPass();
	}
}

Vector2u SceneRenderer::Target::GetSize()
{
	SE_CORE_ASSERT(m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Width == m_CompositePass->GetSpecification().TargetFramebuffer->GetSpecification().Width &&
				   m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Height == m_CompositePass->GetSpecification().TargetFramebuffer->GetSpecification().Height);
	return { m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Width, m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Height };
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

const std::shared_ptr<Texture2D> &SceneRenderer::Target::GetFinalColorBuffer() const
{
	SE_CORE_ASSERT(false, "Not implemented");
	return nullptr;
}

RendererID SceneRenderer::Target::GetFinalColorBufferRendererID() const
{
	return m_CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
}

void SceneRenderer::Target::CalculateCascades(CascadeData *cascades, const Vector3f &lightDirection)
{
	FrustumBounds frustumBounds[3];

	auto &sceneCamera = m_CameraData;
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
	for ( uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++ )
	{
		float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = s_Data.CascadeSplitLambda * (log - uniform) + uniform;
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
	for ( uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++ )
	{
		float splitDist = cascadeSplits[i];

		Vector3f frustumCorners[8] =
		{
			Vector3f(-1.0f, 1.0f, -1.0f),
			Vector3f(1.0f, 1.0f, -1.0f),
			Vector3f(1.0f, -1.0f, -1.0f),
			Vector3f(-1.0f, -1.0f, -1.0f),
			Vector3f(-1.0f, 1.0f, 1.0f),
			Vector3f(1.0f, 1.0f, 1.0f),
			Vector3f(1.0f, -1.0f, 1.0f),
			Vector3f(-1.0f, -1.0f, 1.0f),
		};

		// Project frustum corners into world space
		Matrix4f invCam = glm::inverse(viewProjection);
		for ( uint32_t i = 0; i < 8; i++ )
		{
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for ( uint32_t i = 0; i < 4; i++ )
		{
			Vector3f dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + dist * splitDist;
			frustumCorners[i] = frustumCorners[i] + dist * lastSplitDist;
		}

		// Get frustum center
		Vector3f frustumCenter = Vector3f(0.0f);
		for ( uint32_t i = 0; i < 8; i++ )
			frustumCenter += frustumCorners[i];

		frustumCenter /= 8.0f;

		//frustumCenter *= 0.01f;

		float radius = 0.0f;
		for ( uint32_t i = 0; i < 8; i++ )
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
											   0.0f + s_Data.CascadeNearPlaneOffset,
											   maxExtents.z - minExtents.z + s_Data.CascadeFarPlaneOffset);

		// Store split distance and matrix in cascade
		cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
		cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
		cascades[i].View = lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}
}

std::shared_ptr<SceneRenderer::Target> SceneRenderer::Target::Create(String friendlyName, Uint32 width, Uint32 height)
{
	std::shared_ptr<Target> target = CreateShared<Target>(Move(friendlyName));

	Framebuffer::Specification geoFramebufferSpec;
	geoFramebufferSpec.Width = width;
	geoFramebufferSpec.Height = height;
	geoFramebufferSpec.Attachments = {
		Framebuffer::TextureFormat::RGBA16F,
		Framebuffer::TextureFormat::RGBA16F,
		Framebuffer::TextureFormat::Depth
	};
	geoFramebufferSpec.Samples = 8;
	geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification geoRenderPassSpec;
	geoRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(geoFramebufferSpec);
	target->m_GeoPass = Factory::Create<RenderPass>(geoRenderPassSpec);

	Framebuffer::Specification compFramebufferSpec;
	compFramebufferSpec.Attachments = { Framebuffer::TextureFormat::RGBA8 };
	compFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification compRenderPassSpec;
	compRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(compFramebufferSpec);
	target->m_CompositePass = Factory::Create<RenderPass>(compRenderPassSpec);

	Framebuffer::Specification bloomBlurFramebufferSpec;
	bloomBlurFramebufferSpec.Attachments = { Framebuffer::TextureFormat::RGBA16F };
	bloomBlurFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification bloomBlurRenderPassSpec;
	bloomBlurRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(bloomBlurFramebufferSpec);
	target->m_BloomBlurPass[0] = Factory::Create<RenderPass>(bloomBlurRenderPassSpec);
	bloomBlurRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(bloomBlurFramebufferSpec);
	target->m_BloomBlurPass[1] = Factory::Create<RenderPass>(bloomBlurRenderPassSpec);

	Framebuffer::Specification bloomBlendFramebufferSpec;
	bloomBlendFramebufferSpec.Attachments = { Framebuffer::TextureFormat::RGBA8 };
	bloomBlendFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification bloomBlendRenderPassSpec;
	bloomBlendRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(bloomBlendFramebufferSpec);
	target->m_BloomBlendPass = Factory::Create<RenderPass>(bloomBlendRenderPassSpec);

	// Shadow Map
	Framebuffer::Specification shadowMapFramebufferSpec;
	shadowMapFramebufferSpec.Width = 4096;
	shadowMapFramebufferSpec.Height = 4096;
	shadowMapFramebufferSpec.Attachments = { Framebuffer::TextureFormat::DEPTH32F };
	shadowMapFramebufferSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	shadowMapFramebufferSpec.NoResize = true;

	// 4 cascades
	for ( auto &shadowMapRenderPass : target->m_ShadowMapRenderPass )
	{
		RenderPass::Specification shadowMapRenderPassSpec;
		shadowMapRenderPassSpec.TargetFramebuffer = Factory::Create<Framebuffer>(shadowMapFramebufferSpec);
		shadowMapRenderPass = Factory::Create<RenderPass>(shadowMapRenderPassSpec);
	}

	Renderer::Submit([]()
					 {
						 glGenSamplers(1, &s_Data.ShadowMapSampler);

						 // Setup the shadowmap depth sampler
						 glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						 glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						 glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						 glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					 });


	s_Data.RegisteredRenderTargets.push_back(target);

	target->Enable();

	return target;
}


///////////////////////////////////////////////////////////////////////////
/// Scene Renderer
///////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init()
{
	s_Data.MainRenderTarget = Target::Create("Main", 1280, 720);

	s_Data.CompositeShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/SceneComposite.glsl" });
	s_Data.BloomBlurShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/BloomBlur.glsl" });
	s_Data.BloomBlendShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/BloomBlend.glsl" });
	s_Data.BRDFLUT = Factory::Create<Texture2D>(Filepath{ "Resources/Assets/Textures/BRDF_LUT.tga" });

	// Grid
	const auto gridShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/shaders/Grid.glsl" });
	s_Data.GridMaterial = MaterialInstance::Create(Factory::Create<Material>(gridShader));
	s_Data.GridMaterial->SetFlag(Material::Flag::TwoSided, true);
	const float gridScale = 16.025f, gridSize = 0.025f;
	s_Data.GridMaterial->Set("u_Scale", gridScale);
	s_Data.GridMaterial->Set("u_Res", gridSize);

	// Outline
	const auto outlineShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/Outline.glsl" });
	s_Data.OutlineMaterial = MaterialInstance::Create(Factory::Create<Material>(outlineShader));
	s_Data.OutlineMaterial->SetFlag(Material::Flag::DepthTest, false);

	// Lines
	s_Data.LineShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/Renderer2D_Line.glsl" });
	s_Data.LineVertexBufferBase = new LineVertex[SceneRendererData::MaxLineVertices];

	// Shadow Map
	s_Data.ShadowMapShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/ShadowMap.glsl" });
}

void SceneRenderer::BeginScene(const Scene *scene, ArrayList<std::shared_ptr<Target>> targets)
{
	SE_CORE_ASSERT(!s_Data.ActiveScene, "Already an active scene");

	s_Data.ActiveScene = scene;
	s_Data.RenderTargets = Move(targets);

	s_Data.SceneData.SkyboxMaterial = scene->GetSkybox().Material;
	s_Data.SceneData.SceneEnvironment = scene->GetEnvironment();
	s_Data.SceneData.ActiveLight = scene->GetLight();
	s_Data.SceneData.SceneLightEnvironment = scene->GetLightEnvironment();
	s_Data.LineIndexCount = 0;
	s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
}

void SceneRenderer::EndScene()
{
	SE_CORE_ASSERT(s_Data.ActiveScene, "No active scene");

	s_Data.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::OnGuiRender()
{
	ImGui::Begin("Scene Renderer");

	if ( Gui::BeginTreeNode("Shadows") )
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Soft Shadows", s_Data.SoftShadows);
		Gui::Property("Light Size", s_Data.LightSize, 0.01f);
		Gui::Property("Max Shadow Distance", s_Data.MaxShadowDistance, 1.0f);
		Gui::Property("Shadow Fade", s_Data.ShadowFade, 5.0f);
		Gui::EndPropertyGrid();
		if ( Gui::BeginTreeNode("Cascade Settings") )
		{
			Gui::BeginPropertyGrid();
			Gui::Property("Show Cascades", s_Data.ShowCascades);
			Gui::Property("Cascade Fading", s_Data.CascadeFading);
			Gui::Property("Cascade Transition Fade", s_Data.CascadeTransitionFade, 0.05f, 0.0f, FLT_MAX);
			Gui::Property("Cascade Split", s_Data.CascadeSplitLambda, 0.01f);
			Gui::Property("CascadeNearPlaneOffset", s_Data.CascadeNearPlaneOffset, 0.1f, -FLT_MAX, 0.0f);
			Gui::Property("CascadeFarPlaneOffset", s_Data.CascadeFarPlaneOffset, 0.1f, 0.0f, FLT_MAX);
			Gui::EndPropertyGrid();
			Gui::EndTreeNode();
		}
		if ( Gui::BeginTreeNode("Shadow Map", false) )
		{
			int nodeID = 0;
			for ( const auto &target : s_Data.RegisteredRenderTargets )
			{
				if ( Gui::BeginTreeNode("Target: " + target->GetName() + "##" + std::to_string(nodeID++), false) )
				{
					static int cascadeIndex = 0;
					const auto framebuffer = target->GetShadowMapRenderPass(cascadeIndex)->GetSpecification().TargetFramebuffer;
					const auto rendererID = framebuffer->GetDepthAttachmentRendererID();

					const float regionSize = ImGui::GetContentRegionAvailWidth(); // (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
					Gui::BeginPropertyGrid();
					Gui::Property("Cascade Index", cascadeIndex, 0, 3, 1, Gui::PropertyFlag::Slider);
					Gui::EndPropertyGrid();
					ImGui::Image(reinterpret_cast<ImTextureID>(rendererID), { regionSize, regionSize }, { 0, 1 }, { 1, 0 });
					Gui::EndTreeNode();
				}
			}
			Gui::EndTreeNode();
		}
		Gui::EndTreeNode();
	}
	int nodeID = 0;
	if ( Gui::BeginTreeNode("Bloom") )
	{
		for ( const auto &target : s_Data.RegisteredRenderTargets )
		{
			if ( Gui::BeginTreeNode("Target: " + target->GetName() + "##" + std::to_string(nodeID++), false) )
			{

				Gui::BeginPropertyGrid();
				Gui::Property("Bloom", s_Data.EnableBloom);
				Gui::Property("Bloom threshold", s_Data.BloomThreshold, 0.05f);
				Gui::EndPropertyGrid();

				auto framebuffer = target->GetBloomBlurPass(0)->GetSpecification().TargetFramebuffer;
				const auto rendererID = framebuffer->GetColorAttachmentRendererID();

				const float size = ImGui::GetContentRegionAvailWidth(); // (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
				const float w = size;
				const float h = w / (static_cast<float>(framebuffer->GetWidth()) / static_cast<float>(framebuffer->GetHeight()));
				ImGui::Image(reinterpret_cast<ImTextureID>(rendererID), { w, h }, { 0, 1 }, { 1, 0 });
				Gui::EndTreeNode();
			}
		}
		Gui::EndTreeNode();
	}

	ImGui::End();
}

void SceneRenderer::SubmitMesh(const std::shared_ptr<Mesh> &mesh, const Matrix4f &transform,
							   const std::shared_ptr<MaterialInstance> &overrideMaterial)
{
	// TODO: Culling, sorting, etc.
	s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
	s_Data.ShadowPassDrawList.push_back({ mesh, overrideMaterial, transform });
}

void SceneRenderer::SubmitSelectedMesh(const std::shared_ptr<Mesh> &mesh, const Matrix4f &transform)
{
	s_Data.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
	s_Data.ShadowPassDrawList.push_back({ mesh, nullptr, transform });
}

void SceneRenderer::SubmitLine(const Vector3f &first, const Vector3f &second, const Vector4f &color)
{
	if ( s_Data.LineIndexCount >= SceneRendererData::MaxLineIndices )
	{
		return;
	}

	s_Data.LineVertexBufferPtr->Position = first;
	s_Data.LineVertexBufferPtr->Color = color;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineVertexBufferPtr->Position = second;
	s_Data.LineVertexBufferPtr->Color = color;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineIndexCount += 2;
}

void SceneRenderer::SubmitAABB(std::shared_ptr<Mesh> mesh, const Matrix4f &transform, const Vector4f &color)
{
	for ( const auto &submesh : mesh->GetSubmeshes() )
	{
		const auto &aabb = submesh.BoundingBox;
		auto aabbTransform = transform * mesh->GetLocalTransform() * submesh.Transform;
		SubmitAABB(aabb, aabbTransform);
	}
}

void SceneRenderer::SubmitAABB(const AABB &aabb, const Matrix4f &transform, const Vector4f &color)
{
	Vector4f corners[8] =
	{
		transform * Vector4f { aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f },
		transform * Vector4f { aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f },
		transform * Vector4f { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f },
		transform * Vector4f { aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f },

		transform * Vector4f { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f },
		transform * Vector4f { aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f },
		transform * Vector4f { aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f },
		transform * Vector4f { aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f }
	};

	for ( Uint32 i = 0; i < 4; i++ )
		SubmitLine(corners[i], corners[(i + 1) % 4], color);

	for ( Uint32 i = 0; i < 4; i++ )
		SubmitLine(corners[i + 4], corners[(i + 1) % 4 + 4], color);

	for ( Uint32 i = 0; i < 4; i++ )
		SubmitLine(corners[i], corners[i + 4], color);
}


std::shared_ptr<SceneEnvironment> SceneRenderer::CreateSceneEnvironment(const Filepath &filepath)
{
	const Uint32 cubemapSize = 2048;
	const Uint32 irradianceMapSize = 32;

	std::shared_ptr<TextureCube> envUnfiltered = Factory::Create<TextureCube>(Texture::Format::Float16, cubemapSize, cubemapSize);
	if ( !equirectangularConversionShader )
	{
		equirectangularConversionShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/EquirectangularToCubeMap.glsl" });
	}
	std::shared_ptr<Texture2D> envEquirect = Factory::Create<Texture2D>(filepath);
	SE_CORE_ASSERT(envEquirect->GetFormat() == Texture::Format::Float16, "Texture is not HDR!");

	equirectangularConversionShader->Bind();
	envEquirect->Bind();
	Renderer::Submit([envUnfiltered, cubemapSize, envEquirect]()
					 {
						 glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
						 glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
						 glGenerateTextureMipmap(envUnfiltered->GetRendererID());
					 });

	if ( !envFilteringShader )
		envFilteringShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/EnvironmentMipFilter.glsl" });

	std::shared_ptr<TextureCube> envFiltered = Factory::Create<TextureCube>(Texture::Format::Float16, cubemapSize, cubemapSize);

	Renderer::Submit([envUnfiltered, envFiltered]()
					 {
						 glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
											envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
											envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
					 });

	envFilteringShader->Bind();
	envUnfiltered->Bind();

	Renderer::Submit([envUnfiltered, envFiltered, cubemapSize]()
					 {
						 const float deltaRoughness = 1.0f / glm::max(static_cast<float>(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
						 for ( int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2 ) // <= ?
						 {
							 const GLuint numGroups = glm::max(1, size / 32);
							 glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
							 glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
							 glDispatchCompute(numGroups, numGroups, 6);
						 }
					 });

	if ( !envIrradianceShader )
	{
		envIrradianceShader = Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/EnvironmentIrradiance.glsl" });
	}

	std::shared_ptr<TextureCube> irradianceMap = Factory::Create<TextureCube>(Texture::Format::Float16, irradianceMapSize, irradianceMapSize);
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


std::shared_ptr<SceneRenderer::Target> &SceneRenderer::GetMainTarget()
{
	return s_Data.MainRenderTarget;
}

SceneRenderer::Options &SceneRenderer::GetOptions()
{
	return s_Data.Options;
}

void SceneRenderer::FlushDrawList()
{
	SE_CORE_ASSERT(!s_Data.ActiveScene, "");

	for ( auto &target : s_Data.RenderTargets )
	{
		if ( target->IsEnabled() )
		{
			target->ResetRenderingStats();
			auto &stats = target->GetRenderingStats();
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

	s_Data.DrawList.clear();
	s_Data.SelectedMeshDrawList.clear();
	s_Data.ShadowPassDrawList.clear();
	s_Data.SceneData = {};

}
}

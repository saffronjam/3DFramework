#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Rendering/Passes/ShadowMapPass.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
ShadowMapPass::ShadowMapPass(String name) :
	RenderPass(Move(name)),
	_shader(Shader::Create("ShadowMap"))
{
	FramebufferSpecification shadowMapFramebufferSpec;
	shadowMapFramebufferSpec.Width = 4096;
	shadowMapFramebufferSpec.Height = 4096;
	shadowMapFramebufferSpec.Attachments = {FramebufferTextureFormat::DEPTH32F};
	shadowMapFramebufferSpec.ClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
	shadowMapFramebufferSpec.NoResize = true;

	// 4 cascades
	for (auto& shadowMap : _shadowMaps)
	{
		shadowMap = Framebuffer::Create(shadowMapFramebufferSpec);
	}

	AddOutput(CreateUnique<Stream::Output>("shadowMap0", _shadowMaps[0]));
	AddOutput(CreateUnique<Stream::Output>("shadowMap1", _shadowMaps[1]));
	AddOutput(CreateUnique<Stream::Output>("shadowMap2", _shadowMaps[2]));
	AddOutput(CreateUnique<Stream::Output>("shadowMap3", _shadowMaps[3]));
}

void ShadowMapPass::Execute()
{	
	auto& sceneInfo = SceneRenderer::GetSceneInfo();
	auto& shadowMapMeshes = SceneRenderer::GetDrawCommands(RenderChannel::Shadow);
	auto &smData = SceneRenderer::GetShadowMapData();

	auto& directionalLights = sceneInfo.LightEnvironment.DirectionalLights;
	if (directionalLights[0].Multiplier == 0.0f || !directionalLights[0].CastShadows)
	{
		for (int i = 0; i < 4; i++)
		{
			// Clear shadow maps
			Renderer::Begin(_shadowMaps[i]);
			Renderer::End();
		}
		return;
	}

	CascadeData cascades[4];
	CalculateCascades(cascades, directionalLights[0].Direction);
	smData.LightViewMatrix = cascades[0].View;

	Renderer::Submit([]()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	});

	for (int i = 0; i < 4; i++)
	{
		smData.CascadeSplits[i] = cascades[i].SplitDepth;

		Renderer::Begin(_shadowMaps[i]);

		Matrix4 shadowMapVP = cascades[i].ViewProj;
		_shader->SetMat4("u_ViewProjection", shadowMapVP);

		static Matrix4 scaleBiasMatrix = scale(Matrix4(1.0f), {0.5f, 0.5f, 0.5f}) * translate(
			Matrix4(1.0f), {1, 1, 1});
		smData.LightMatrices[i] = scaleBiasMatrix * cascades[i].ViewProj;


		// Render entities
		for (auto& dc : shadowMapMeshes)
		{
			Renderer::SubmitMeshWithShader(dc.Mesh, dc.Transform, _shader);
		}

		Renderer::End();
	}
}

void ShadowMapPass::OnViewportResize(uint width, uint height)
{
}

void ShadowMapPass::CalculateCascades(CascadeData* cascades, const Vector3& lightDirection)
{
	auto& sceneInfo = SceneRenderer::GetSceneInfo();
	auto &smData = SceneRenderer::GetShadowMapData();

	FrustumBounds frustumBounds[3];

	auto& sceneCamera = sceneInfo.SceneCamera;
	auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;

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
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = smData.CascadeSplitLambda * (log - uniform) + uniform;
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
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float splitDist = cascadeSplits[i];

		Vector3 frustumCorners[8] = {
			Vector3(-1.0f, 1.0f, -1.0f), Vector3(1.0f, 1.0f, -1.0f), Vector3(1.0f, -1.0f, -1.0f),
			Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f),
			Vector3(1.0f, -1.0f, 1.0f), Vector3(-1.0f, -1.0f, 1.0f),
		};

		// Project frustum corners into world space
		Matrix4 invCam = inverse(viewProjection);
		for (uint i = 0; i < 8; i++)
		{
			Vector4 invCorner = invCam * Vector4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for (uint i = 0; i < 4; i++)
		{
			Vector3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		Vector3 frustumCenter = Vector3(0.0f);
		for (uint i = 0; i < 8; i++) frustumCenter += frustumCorners[i];

		frustumCenter /= 8.0f;

		//frustumCenter *= 0.01f;

		float radius = 0.0f;
		for (uint i = 0; i < 8; i++)
		{
			float distance = length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		Vector3 maxExtents(radius);
		Vector3 minExtents = -maxExtents;

		Vector3 lightDir = -lightDirection;
		Matrix4 lightViewMatrix = lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter,
		                                  Vector3(0.0f, 0.0f, 1.0f));
		Matrix4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y,
		                                       0.0f + smData.CascadeNearPlaneOffset,
		                                       maxExtents.z - minExtents.z + smData.CascadeFarPlaneOffset);

		// Store split distance and matrix in cascade
		cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
		cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
		cascades[i].View = lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}
}
}

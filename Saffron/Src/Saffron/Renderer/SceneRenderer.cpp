#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Renderer2D.h"
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
		Shared<MaterialInstance> SkyboxMaterial;
		Scene::Environment SceneEnvironment;
		Scene::Light ActiveLight;
	} SceneData;

	Shared<Texture2D> BRDFLUT;
	Shared<Shader> CompositeShader;

	struct RenderTarget
	{
		bool Enabled;
		SceneRendererCameraData CameraData;
		Shared<RenderPass> GeoPass;
		Shared<RenderPass> CompositePass;
	};
	std::map<std::string, RenderTarget> RenderTargets;

	struct DrawCommand
	{
		Shared<Mesh> Mesh;
		Shared<MaterialInstance> Material;
		glm::mat4 Transform;
	};
	std::vector<DrawCommand> DrawList;
	std::vector<DrawCommand> SelectedMeshDrawList;

	// Grid
	Shared<MaterialInstance> GridMaterial;
	Shared<MaterialInstance> OutlineMaterial;

	SceneRenderer::Options Options;

};

static SceneRendererData s_Data;
static Shared<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

///////////////////////////////////////////////////////////////////////////
/// Scene Renderer
///////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init()
{
	AddRenderTarget("MainRenderTarget", 1280, 720);

	s_Data.CompositeShader = Shader::Create("Assets/Shaders/SceneComposite.glsl");
	const auto outlineShader = Shader::Create("Assets/Shaders/Outline.glsl");
	s_Data.OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
	s_Data.BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga");

	// Grid
	const auto gridShader = Shader::Create("Assets/Shaders/Grid.glsl");
	s_Data.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
	const float gridScale = 16.025f, gridSize = 0.025f;
	s_Data.GridMaterial->Set("u_Scale", gridScale);
	s_Data.GridMaterial->Set("u_Res", gridSize);

	// Outline
	s_Data.OutlineMaterial->SetFlag(Material::Flag::DepthTest, false);
}

void SceneRenderer::AddRenderTarget(const std::string &renderTargetIdentifier, Uint32 width, Uint32 height)
{
	Framebuffer::Specification geoFramebufferSpec;
	geoFramebufferSpec.Width = width;
	geoFramebufferSpec.Height = height;
	geoFramebufferSpec.Format = Framebuffer::Format::RGBA16F;
	geoFramebufferSpec.Samples = 8;
	geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification geoRenderPassSpec;
	geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
	s_Data.RenderTargets[renderTargetIdentifier].GeoPass = RenderPass::Create(geoRenderPassSpec);

	Framebuffer::Specification compFramebufferSpec;
	compFramebufferSpec.Width = 1280;
	compFramebufferSpec.Height = 720;
	compFramebufferSpec.Format = Framebuffer::Format::RGBA8;
	compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification compRenderPassSpec;
	compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
	s_Data.RenderTargets[renderTargetIdentifier].CompositePass = RenderPass::Create(compRenderPassSpec);

	EnableRenderTarget(renderTargetIdentifier);
}

void SceneRenderer::SetRenderTargetSize(const std::string &renderTargetIdentifier, Uint32 width, Uint32 height)
{
	s_Data.RenderTargets[renderTargetIdentifier].GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
	s_Data.RenderTargets[renderTargetIdentifier].CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
}

void SceneRenderer::SetCameraData(const std::string &renderTargetIdentifier, const SceneRendererCameraData &cameraData)
{
	s_Data.RenderTargets[renderTargetIdentifier].CameraData = cameraData;
}

void SceneRenderer::EnableRenderTarget(const std::string &renderTargetIdentifier)
{
	s_Data.RenderTargets[renderTargetIdentifier].Enabled = true;
}

void SceneRenderer::DisableRenderTarget(const std::string &renderTargetIdentifier)
{
	s_Data.RenderTargets[renderTargetIdentifier].Enabled = false;
}

bool SceneRenderer::IsRenderTargetEnabled(const std::string &renderTargetIdentifier)
{
	return s_Data.RenderTargets[renderTargetIdentifier].Enabled;
}

void SceneRenderer::BeginScene(const Scene *scene)
{
	SE_CORE_ASSERT(!s_Data.ActiveScene, "Already an active scene");

	s_Data.ActiveScene = scene;
	s_Data.SceneData.SkyboxMaterial = scene->GetSkybox().Material;
	s_Data.SceneData.SceneEnvironment = scene->GetEnvironment();
	s_Data.SceneData.ActiveLight = scene->GetLight();
}

void SceneRenderer::EndScene()
{
	SE_CORE_ASSERT(s_Data.ActiveScene, "No active scene");

	s_Data.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::SubmitMesh(const Shared<Mesh> &mesh, const glm::mat4 &transform,
							   const Shared<MaterialInstance> &overrideMaterial)
{
	// TODO: Culling, sorting, etc.
	s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
}

void SceneRenderer::SubmitSelectedMesh(const Shared<Mesh> &mesh, const glm::mat4 &transform)
{
	s_Data.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
}

std::pair<Shared<TextureCube>, Shared<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string &filepath)
{
	const Uint32 cubemapSize = 2048;
	const Uint32 irradianceMapSize = 32;

	Shared<TextureCube> envUnfiltered = TextureCube::Create(Texture::Format::Float16, cubemapSize, cubemapSize);
	if ( !equirectangularConversionShader )
		equirectangularConversionShader = Shader::Create("Assets/Shaders/EquirectangularToCubeMap.glsl");
	Shared<Texture2D> envEquirect = Texture2D::Create(filepath);
	SE_CORE_ASSERT(envEquirect->GetFormat() == Texture::Format::Float16, "Texture is not HDR!");

	equirectangularConversionShader->Bind();
	envEquirect->Bind();
	Renderer::Submit([envUnfiltered, cubemapSize]()
					 {
						 glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
						 glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
						 glGenerateTextureMipmap(envUnfiltered->GetRendererID());
					 });


	if ( !envFilteringShader )
		envFilteringShader = Shader::Create("Assets/Shaders/EnvironmentMipFilter.glsl");

	Shared<TextureCube> envFiltered = TextureCube::Create(Texture::Format::Float16, cubemapSize, cubemapSize);

	Renderer::Submit([envUnfiltered, envFiltered]()
					 {
						 glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
											envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
											envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
					 });

	envFilteringShader->Bind();
	envUnfiltered->Bind();

	Renderer::Submit([envFiltered, cubemapSize]() {
		const float deltaRoughness = 1.0f / glm::max(static_cast<float>(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
		for ( int level = 1, size = cubemapSize / 2; level < static_cast<Int32>(envFiltered->GetMipLevelCount()); level++, size /= 2 ) // <= ?
		{
			const GLuint numGroups = glm::max(1, size / 32);
			glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
			glDispatchCompute(numGroups, numGroups, 6);
		}
					 });

	if ( !envIrradianceShader )
		envIrradianceShader = Shader::Create("Assets/Shaders/EnvironmentIrradiance.glsl");

	Shared<TextureCube> irradianceMap = TextureCube::Create(Texture::Format::Float16, irradianceMapSize, irradianceMapSize);
	envIrradianceShader->Bind();
	envFiltered->Bind();
	Renderer::Submit([irradianceMap]()
					 {
						 glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
						 glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
						 glGenerateTextureMipmap(irradianceMap->GetRendererID());
					 });

	return { envFiltered, irradianceMap };
}

Shared<RenderPass> SceneRenderer::GetFinalRenderPass(const std::string &renderTargetIdentifier)
{
	SE_CORE_ASSERT(s_Data.RenderTargets.find(renderTargetIdentifier) != s_Data.RenderTargets.end());
	return s_Data.RenderTargets[renderTargetIdentifier].CompositePass;
}

Shared<Texture2D> SceneRenderer::GetFinalColorBuffer(const std::string &renderTargetIdentifier)
{
	SE_CORE_ASSERT(s_Data.RenderTargets.find(renderTargetIdentifier) != s_Data.RenderTargets.end());
	// return s_Data.CompositePass->GetSpecification().TargetFramebuffer;
	SE_CORE_ASSERT(false, "Not implemented");
	return nullptr;
}

Uint32 SceneRenderer::GetFinalColorBufferRendererID(const std::string &renderTargetIdentifier)
{
	SE_ASSERT(s_Data.RenderTargets.find(renderTargetIdentifier) != s_Data.RenderTargets.end());
	return s_Data.RenderTargets[renderTargetIdentifier].CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
}

SceneRenderer::Options &SceneRenderer::GetOptions()
{
	return s_Data.Options;
}

void SceneRenderer::FlushDrawList()
{
	SE_CORE_ASSERT(!s_Data.ActiveScene, "");

	for ( auto &[renderTargetIdentifier, rendererPass] : s_Data.RenderTargets )
	{
		if ( s_Data.RenderTargets[renderTargetIdentifier].Enabled )
		{
			GeometryPass(renderTargetIdentifier);
			CompositePass(renderTargetIdentifier);
		}
	}
	s_Data.DrawList.clear();
	s_Data.SelectedMeshDrawList.clear();
	s_Data.SceneData = {};
}

void SceneRenderer::GeometryPass(const std::string &renderTargetIdentifier)
{
	const SceneRendererCameraData &camData = s_Data.RenderTargets[renderTargetIdentifier].CameraData;
	if ( !camData.Camera )
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

	Renderer::BeginRenderPass(s_Data.RenderTargets[renderTargetIdentifier].GeoPass);


	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilMask(0);
						 });
	}
	const auto viewProjection = camData.Camera->GetProjectionMatrix() * camData.ViewMatrix;
	const glm::vec3 cameraPosition = glm::inverse(camData.ViewMatrix)[3];

	// Skybox
	auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
	s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
	Renderer::SubmitQuad(s_Data.SceneData.SkyboxMaterial);

	// Render entities
	for ( auto &dc : s_Data.DrawList )
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_CameraPosition", cameraPosition);

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
		baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
		baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

		const auto overrideMaterial = nullptr; // dc.Material;
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
		baseMaterial->Set("u_CameraPosition", cameraPosition);

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
		baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
		baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

		const auto overrideMaterial = nullptr; // dc.Material;

		Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
	}

	if ( outline )
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
		Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
	}

	if ( GetOptions().ShowBoundingBoxes )
	{
		Renderer2D::BeginScene(viewProjection);
		for ( auto &dc : s_Data.DrawList )
			Renderer::DrawAABB(dc.Mesh, dc.Transform);
		Renderer2D::EndScene();
	}

	Renderer::EndRenderPass();
}

void SceneRenderer::CompositePass(const std::string &renderTargetIdentifier)
{
	const SceneRendererCameraData &camData = s_Data.RenderTargets[renderTargetIdentifier].CameraData;
	if ( !camData.Camera )
	{
		return;
	}
	Renderer::BeginRenderPass(s_Data.RenderTargets[renderTargetIdentifier].CompositePass);
	s_Data.CompositeShader->Bind();
	s_Data.CompositeShader->SetFloat("u_Exposure", camData.Camera->GetExposure());
	s_Data.CompositeShader->SetInt("u_TextureSamples", s_Data.RenderTargets[renderTargetIdentifier].GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);

	s_Data.RenderTargets[renderTargetIdentifier].GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
	Renderer::SubmitQuad(nullptr);
	Renderer::EndRenderPass();
}
}

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
		SceneRenderer::SceneCamera SceneCamera;

		// Resources
		Ref<MaterialInstance> SkyboxMaterial;
		Environment SceneEnvironment;
		Light ActiveLight;
	} SceneData;

	Ref<Texture2D> BRDFLUT;
	Ref<Shader> CompositeShader;

	Ref<RenderPass> GeoPass;
	Ref<RenderPass> CompositePass;

	struct DrawCommand
	{
		Ref<Mesh> Mesh;
		Ref<MaterialInstance> Material;
		glm::mat4 Transform;
	};
	std::vector<DrawCommand> DrawList;
	std::vector<DrawCommand> SelectedMeshDrawList;

	// Grid
	Ref<MaterialInstance> GridMaterial;
	Ref<MaterialInstance> OutlineMaterial;

	SceneRenderer::Options Options;
};

static SceneRendererData sData;
static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;


///////////////////////////////////////////////////////////////////////////
/// Scene Renderer
///////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init()
{
	Framebuffer::Specification geoFramebufferSpec;
	geoFramebufferSpec.Width = 1280;
	geoFramebufferSpec.Height = 720;
	geoFramebufferSpec.Format = Framebuffer::Format::RGBA16F;
	geoFramebufferSpec.Samples = 8;
	geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification geoRenderPassSpec;
	geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
	sData.GeoPass = RenderPass::Create(geoRenderPassSpec);

	Framebuffer::Specification compFramebufferSpec;
	compFramebufferSpec.Width = 1280;
	compFramebufferSpec.Height = 720;
	compFramebufferSpec.Format = Framebuffer::Format::RGBA8;
	compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification compRenderPassSpec;
	compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
	sData.CompositePass = RenderPass::Create(compRenderPassSpec);

	sData.CompositeShader = Shader::Create("Assets/Shaders/SceneComposite.glsl");
	sData.BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga");

	// Grid
	const auto gridShader = Shader::Create("Assets/Shaders/Grid.glsl");
	sData.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
	const float gridScale = 16.025f, gridSize = 0.025f;
	sData.GridMaterial->Set("u_Scale", gridScale);
	sData.GridMaterial->Set("u_Res", gridSize);

	// Outline
	const auto outlineShader = Shader::Create("Assets/Shaders/Outline.glsl");
	sData.OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
	sData.OutlineMaterial->SetFlag(Material::Flag::DepthTest, false);
}

void SceneRenderer::SetViewportSize(Uint32 width, Uint32 height)
{
	sData.GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
	sData.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
}

void SceneRenderer::BeginScene(const Scene *scene, const SceneCamera &camera)
{
	SE_CORE_ASSERT(!sData.ActiveScene, "No active scene");

	sData.ActiveScene = scene;

	sData.SceneData.SceneCamera = camera;
	sData.SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
	sData.SceneData.SceneEnvironment = scene->m_Environment;
	sData.SceneData.ActiveLight = scene->m_Light;
}

void SceneRenderer::EndScene()
{
	SE_CORE_ASSERT(sData.ActiveScene, "No active scene");

	sData.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::SubmitMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform,
							   const Ref<MaterialInstance> &overrideMaterial)
{
	// TODO: Culling, sorting, etc.
	sData.DrawList.push_back({ mesh, overrideMaterial, transform });
}

void SceneRenderer::SubmitSelectedMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform)
{
	sData.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
}

std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string &filepath)
{
	const Uint32 cubemapSize = 2048;
	const Uint32 irradianceMapSize = 32;

	Ref<TextureCube> envUnfiltered = TextureCube::Create(Texture::Format::Float16, cubemapSize, cubemapSize);
	if ( !equirectangularConversionShader )
		equirectangularConversionShader = Shader::Create("Assets/Shaders/EquirectangularToCubeMap.glsl");
	Ref<Texture2D> envEquirect = Texture2D::Create(filepath);
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

	Ref<TextureCube> envFiltered = TextureCube::Create(Texture::Format::Float16, cubemapSize, cubemapSize);

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

	Ref<TextureCube> irradianceMap = TextureCube::Create(Texture::Format::Float16, irradianceMapSize, irradianceMapSize);
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

Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
{
	return sData.CompositePass;
}

Ref<Texture2D> SceneRenderer::GetFinalColorBuffer()
{
	// return sData.CompositePass->GetSpecification().TargetFramebuffer;
	SE_CORE_ASSERT(false, "Not implemented");
	return nullptr;
}

Uint32 SceneRenderer::GetFinalColorBufferRendererID()
{
	return sData.CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
}

SceneRenderer::Options &SceneRenderer::GetOptions()
{
	return sData.Options;
}

void SceneRenderer::FlushDrawList()
{
	SE_CORE_ASSERT(!sData.ActiveScene, "");

	GeometryPass();
	CompositePass();

	sData.DrawList.clear();
	sData.SelectedMeshDrawList.clear();
	sData.SceneData = {};
}

void SceneRenderer::GeometryPass()
{
	const bool outline = !sData.SelectedMeshDrawList.empty();

	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
						 });
	}

	Renderer::BeginRenderPass(sData.GeoPass);

	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilMask(0);
						 });
	}

	const auto viewProjection = sData.SceneData.SceneCamera.Camera.GetProjectionMatrix() * sData.SceneData.SceneCamera.ViewMatrix;
	const glm::vec3 cameraPosition = glm::inverse(sData.SceneData.SceneCamera.ViewMatrix)[3];

	// Skybox
	// TODO: Remove this unused line
	auto skyboxShader = sData.SceneData.SkyboxMaterial->GetShader();
	sData.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
	Renderer::SubmitFullscreenQuad(sData.SceneData.SkyboxMaterial);

	// Render entities
	for ( auto &dc : sData.DrawList )
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_CameraPosition", cameraPosition);

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", sData.SceneData.SceneEnvironment.RadianceMap);
		baseMaterial->Set("u_EnvIrradianceTex", sData.SceneData.SceneEnvironment.IrradianceMap);
		baseMaterial->Set("u_BRDFLUTTexture", sData.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("lights", sData.SceneData.ActiveLight);

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
	for ( auto &dc : sData.SelectedMeshDrawList )
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_CameraPosition", cameraPosition);

		// Environment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", sData.SceneData.SceneEnvironment.RadianceMap);
		baseMaterial->Set("u_EnvIrradianceTex", sData.SceneData.SceneEnvironment.IrradianceMap);
		baseMaterial->Set("u_BRDFLUTTexture", sData.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("lights", sData.SceneData.ActiveLight);

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
		sData.OutlineMaterial->Set("u_ViewProjection", viewProjection);
		for ( auto &dc : sData.SelectedMeshDrawList )
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, sData.OutlineMaterial);
		}

		Renderer::Submit([]()
						 {
							 glPointSize(10);
							 glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
						 });
		for ( auto &dc : sData.SelectedMeshDrawList )
		{
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, sData.OutlineMaterial);
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
		sData.GridMaterial->Set("u_ViewProjection", viewProjection);
		Renderer::SubmitQuad(sData.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
	}

	if ( GetOptions().ShowBoundingBoxes )
	{
		Renderer2D::BeginScene(viewProjection);
		for ( auto &dc : sData.DrawList )
			Renderer::DrawAABB(dc.Mesh, dc.Transform);
		Renderer2D::EndScene();
	}

	Renderer::EndRenderPass();
}

void SceneRenderer::CompositePass()
{
	Renderer::BeginRenderPass(sData.CompositePass);
	sData.CompositeShader->Bind();
	sData.CompositeShader->SetFloat("u_Exposure", sData.SceneData.SceneCamera.Camera.GetExposure());
	sData.CompositeShader->SetInt("u_TextureSamples", sData.GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
	sData.GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
	Renderer::SubmitFullscreenQuad(nullptr);
	Renderer::EndRenderPass();
}
}

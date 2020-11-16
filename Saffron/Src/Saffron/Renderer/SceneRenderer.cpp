#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Scene/Scene.h"
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

	Shared<SceneRenderer::Target> MainRenderTarget;
	ArrayList<Shared<SceneRenderer::Target>> RenderTargets;

	struct DrawCommand
	{
		Shared<Mesh> Mesh;
		Shared<MaterialInstance> Material;
		Matrix4f Transform;
	};
	ArrayList<DrawCommand> DrawList;
	ArrayList<DrawCommand> SelectedMeshDrawList;

	// Grid
	Shared<MaterialInstance> GridMaterial;
	Shared<MaterialInstance> OutlineMaterial;

	SceneRenderer::Options Options;

	// Lines
	static constexpr Uint32 MaxLines = 10000;
	static constexpr Uint32 MaxLineVertices = MaxLines * 2;
	static constexpr Uint32 MaxLineIndices = MaxLines * 2;

	Uint32 LineIndexCount = 0;
	LineVertex *LineVertexBufferBase = nullptr;
	LineVertex *LineVertexBufferPtr = nullptr;

	Shared<Shader> LineShader;
};

static SceneRendererData s_Data;
static Shared<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

///////////////////////////////////////////////////////////////////////////
/// Scene Renderer Target
///////////////////////////////////////////////////////////////////////////

SceneRenderer::Target::~Target()
{
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

const Shared<Texture2D> &SceneRenderer::Target::GetFinalColorBuffer() const
{
	SE_CORE_ASSERT(false, "Not implemented");
	return nullptr;
}

RendererID SceneRenderer::Target::GetFinalColorBufferRendererID() const
{
	return m_CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
}

Shared<SceneRenderer::Target> SceneRenderer::Target::Create(Uint32 width, Uint32 height)
{
	Shared<Target> target = Shared<Target>::Create();

	Framebuffer::Specification geoFramebufferSpec;
	geoFramebufferSpec.Width = width;
	geoFramebufferSpec.Height = height;
	geoFramebufferSpec.Format = Framebuffer::Format::RGBA16F;
	geoFramebufferSpec.Samples = 8;
	geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification geoRenderPassSpec;
	geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
	target->m_GeoPass = RenderPass::Create(geoRenderPassSpec);

	Framebuffer::Specification compFramebufferSpec;
	compFramebufferSpec.Width = 1280;
	compFramebufferSpec.Height = 720;
	compFramebufferSpec.Format = Framebuffer::Format::RGBA8;
	compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

	RenderPass::Specification compRenderPassSpec;
	compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
	target->m_CompositePass = RenderPass::Create(compRenderPassSpec);

	s_Data.RenderTargets.push_back(target);

	target->Enable();

	return target;
}


///////////////////////////////////////////////////////////////////////////
/// Scene Renderer
///////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init()
{
	s_Data.MainRenderTarget = Target::Create(1280, 720);

	s_Data.CompositeShader = Shader::Create(Filepath{ "Assets/Shaders/SceneComposite.glsl" });
	const auto outlineShader = Shader::Create(Filepath{ "Assets/Shaders/Outline.glsl" });
	s_Data.OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
	s_Data.BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga");

	// Grid
	const auto gridShader = Shader::Create(Filepath{ "Assets/Shaders/Grid.glsl" });
	s_Data.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
	const float gridScale = 16.025f, gridSize = 0.025f;
	s_Data.GridMaterial->Set("u_Scale", gridScale);
	s_Data.GridMaterial->Set("u_Res", gridSize);

	// Outline
	s_Data.OutlineMaterial->SetFlag(Material::Flag::DepthTest, false);

	// Lines
	{
		s_Data.LineShader = Shader::Create(Filepath{ "Assets/Shaders/Renderer2D_Line.glsl" });
		s_Data.LineVertexBufferBase = new LineVertex[SceneRendererData::MaxLineVertices];
	}

}

void SceneRenderer::BeginScene(const Scene *scene, ArrayList<Shared<Target>> targets)
{
	SE_CORE_ASSERT(!s_Data.ActiveScene, "Already an active scene");

	s_Data.ActiveScene = scene;
	s_Data.RenderTargets = Move(targets);
	s_Data.SceneData.SkyboxMaterial = scene->GetSkybox().Material;
	s_Data.SceneData.SceneEnvironment = scene->GetEnvironment();
	s_Data.SceneData.ActiveLight = scene->GetLight();
	s_Data.LineIndexCount = 0;
	s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
}

void SceneRenderer::EndScene()
{
	SE_CORE_ASSERT(s_Data.ActiveScene, "No active scene");

	s_Data.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::SubmitMesh(const Shared<Mesh> &mesh, const Matrix4f &transform,
							   const Shared<MaterialInstance> &overrideMaterial)
{
	// TODO: Culling, sorting, etc.
	s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
}

void SceneRenderer::SubmitSelectedMesh(const Shared<Mesh> &mesh, const Matrix4f &transform)
{
	s_Data.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
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

void SceneRenderer::SubmitAABB(Shared<Mesh> mesh, const Matrix4f &transform, const Vector4f &color)
{
	for ( const auto &submesh : mesh->GetSubmeshes() )
	{
		const auto &aabb = submesh.BoundingBox;
		auto aabbTransform = transform * submesh.Transform;
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
		SubmitLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

	for ( Uint32 i = 0; i < 4; i++ )
		SubmitLine(corners[i], corners[i + 4], color);
}

Pair<Shared<TextureCube>, Shared<TextureCube>> SceneRenderer::CreateEnvironmentMap(const String &filepath)
{
	const Uint32 cubemapSize = 2048;
	const Uint32 irradianceMapSize = 32;

	Shared<TextureCube> envUnfiltered = TextureCube::Create(Texture::Format::Float16, cubemapSize, cubemapSize);
	if ( !equirectangularConversionShader )
		equirectangularConversionShader = Shader::Create(Filepath{ "Assets/Shaders/EquirectangularToCubeMap.glsl" });
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
		envFilteringShader = Shader::Create(Filepath{ "Assets/Shaders/EnvironmentMipFilter.glsl" });

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
		envIrradianceShader = Shader::Create(Filepath{ "Assets/Shaders/EnvironmentIrradiance.glsl" });

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

Shared<SceneRenderer::Target> &SceneRenderer::GetMainTarget()
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
			GeometryPass(target);
			CompositePass(target);
		}
	}
	s_Data.DrawList.clear();
	s_Data.SelectedMeshDrawList.clear();
	s_Data.SceneData = {};
}

void SceneRenderer::GeometryPass(const Shared<Target> &target)
{
	const CameraData &camData = target->GetCameraData();
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

	Renderer::BeginRenderPass(target->GetGeoPass());


	if ( outline )
	{
		Renderer::Submit([]()
						 {
							 glStencilMask(0);
						 });
	}
	const auto viewProjection = camData.Camera->GetProjectionMatrix() * camData.ViewMatrix;
	const Vector3f cameraPosition = glm::inverse(camData.ViewMatrix)[3];

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
	}

	s_Data.LineShader->SetMat4("u_ViewProjection", viewProjection);
	Renderer::SubmitLines(s_Data.LineVertexBufferBase, s_Data.LineIndexCount, s_Data.LineShader);

	Renderer::EndRenderPass();
}

void SceneRenderer::CompositePass(const Shared<Target> &target)
{
	const CameraData &cameraData = target->GetCameraData();
	if ( !cameraData.Camera )
	{
		return;
	}
	Renderer::BeginRenderPass(target->GetCompositePass());
	s_Data.CompositeShader->Bind();
	s_Data.CompositeShader->SetFloat("u_Exposure", cameraData.Camera->GetExposure());
	s_Data.CompositeShader->SetInt("u_TextureSamples", target->GetGeoPass()->GetSpecification().TargetFramebuffer->GetSpecification().Samples);

	target->GetGeoPass()->GetSpecification().TargetFramebuffer->BindTexture();
	Renderer::SubmitQuad(nullptr);
	Renderer::EndRenderPass();
}
}

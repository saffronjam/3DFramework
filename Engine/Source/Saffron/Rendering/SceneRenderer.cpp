#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneEnvironment.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/Graphs/DefaultRenderGraph.h"

namespace Se
{
SceneRenderer::SceneRenderer() :
	SingleTon(this),
	_sceneInfo(),
	_renderGraph(CreateUnique<DefaultRenderGraph>())
{
	// Grid
	auto gridShader = Shader::Create("Grid");
	_common.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
	_common.GridMaterial->SetFlag(MaterialFlag::TwoSided, true);
	float gridScale = 16.025f, gridSize = 0.025f;
	_common.GridMaterial->Set("u_Scale", gridScale);
	_common.GridMaterial->Set("u_Res", gridSize);

	// Outline
	auto outlineShader = Shader::Create("Outline");
	_common.OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
	_common.OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);
}

void SceneRenderer::OnGuiRender()
{
	auto& common = Instance().GetCommon();
	auto& smData = Instance().GetShadowMapData();

	ImGui::Begin("Scene Renderer");

	if (Gui::BeginTreeNode("Shadows"))
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Soft Shadows", smData.SoftShadows);
		Gui::Property("Light Size", smData.LightSize, 0.01f);
		Gui::Property("Max Shadow Distance", smData.MaxShadowDistance, 1.0f);
		Gui::Property("Shadow Fade", smData.ShadowFade, 5.0f);
		Gui::EndPropertyGrid();
		if (Gui::BeginTreeNode("Cascade Settings"))
		{
			Gui::BeginPropertyGrid();
			Gui::Property("Show Cascades", smData.ShowCascades);
			Gui::Property("Cascade Fading", smData.CascadeFading);
			Gui::Property("Cascade Transition Fade", smData.CascadeTransitionFade, 0.05f, 0.0f, FLT_MAX);
			Gui::Property("Cascade Split", smData.CascadeSplitLambda, 0.01f);
			Gui::Property("CascadeNearPlaneOffset", smData.CascadeNearPlaneOffset, 0.1f, -FLT_MAX, 0.0f);
			Gui::Property("CascadeFarPlaneOffset", smData.CascadeFarPlaneOffset, 0.1f, 0.0f, FLT_MAX);
			Gui::EndPropertyGrid();
			Gui::EndTreeNode();
		}
		if (Gui::BeginTreeNode("Shadow Map", false))
		{
			//static int cascadeIndex = 0;
			//auto fb = common.ShadowFramebuffers[cascadeIndex];
			//auto id = fb->GetDepthAttachmentRendererID();

			//float size = ImGui::GetContentRegionAvailWidth();
			//// (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
			//Gui::BeginPropertyGrid();
			//Gui::Property("Cascade Index", cascadeIndex, 1, 1, 3, Gui::PropertyFlag::Slider);
			//Gui::EndPropertyGrid();
			//ImGui::Image((ImTextureID)id, {size, size}, {0, 1}, {1, 0});
			Gui::EndTreeNode();
		}

		Gui::EndTreeNode();
	}

	if (Gui::BeginTreeNode("Bloom"))
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Bloom", common.EnableBloom);
		Gui::Property("Bloom threshold", common.BloomThreshold, 0.05f);
		Gui::EndPropertyGrid();

		//auto fb = Instance()._renderGraph->GetRenderPass("bloomBlur").;
		//auto id = fb->GetColorAttachmentRendererID();

		//float size = ImGui::GetContentRegionAvailWidth(); // (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
		//float w = size;
		//float h = w / (static_cast<float>(fb->GetWidth()) / static_cast<float>(fb->GetHeight()));
		//ImGui::Image((ImTextureID)id, {w, h}, {0, 1}, {1, 0});
		Gui::EndTreeNode();
	}


	ImGui::End();
}

void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
{
	auto& sceneInfo = GetSceneInfo();

	Debug::Assert(!sceneInfo.ActiveScene, "");

	ResetDrawContainer();

	sceneInfo.ActiveScene = scene;

	sceneInfo.SceneCamera = camera;
	sceneInfo.SkyboxMaterial = scene->GetSkybox().Material;
	sceneInfo.SceneEnvironment = scene->GetSceneEnvironment();
	sceneInfo.SceneEnvironmentIntensity = scene->GetSceneEnvironment()->GetIntensity();
	sceneInfo.ActiveLight = scene->GetLight();
	sceneInfo.LightEnvironment = scene->GetLightEnvironment();
}

void SceneRenderer::EndScene()
{
	auto& sceneInfo = GetSceneInfo();

	Debug::Assert(sceneInfo.ActiveScene, "");

	sceneInfo.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::SubmitMesh(Shared<Mesh> mesh, const Matrix4f& transform, Shared<MaterialInstance> overrideMaterial)
{
	auto& drawContaier = Instance()._drawContainer;
	// TODO: Culling, sorting, etc.
	drawContaier.at(RenderChannel::Main).push_back({mesh, overrideMaterial, transform});
	drawContaier.at(RenderChannel::Shadow).push_back({mesh, overrideMaterial, transform});
}

void SceneRenderer::SubmitSelectedMesh(Shared<Mesh> mesh, const Matrix4f& transform)
{
	auto& drawContaier = Instance()._drawContainer;
	drawContaier.at(RenderChannel::Outline).push_back({mesh, nullptr, transform});
	drawContaier.at(RenderChannel::Shadow).push_back({mesh, nullptr, transform});
}

static Shared<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

Shared<SceneEnvironment> SceneRenderer::CreateEnvironmentMap(const Filepath& filepath)
{
	const Uint32 cubemapSize = 2048;
	const Uint32 irradianceMapSize = 32;

	Shared<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
	if (!equirectangularConversionShader) equirectangularConversionShader = Shader::Create("EquirectangularToCubeMap");
	Shared<Texture2D> envEquirect = Texture2D::Create(filepath);
	Debug::Assert(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

	equirectangularConversionShader->Bind();
	envEquirect->Bind();
	Renderer::Submit([envUnfiltered, cubemapSize, envEquirect]()
	{
		glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
		glGenerateTextureMipmap(envUnfiltered->GetRendererID());
	});

	if (!envFilteringShader) envFilteringShader = Shader::Create("EnvironmentMipFilter");

	Shared<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);

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

	if (!envIrradianceShader) envIrradianceShader = Shader::Create("EnvironmentIrradiance");

	Shared<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, irradianceMapSize,
	                                                        irradianceMapSize);
	envIrradianceShader->Bind();
	envFiltered->Bind();
	Renderer::Submit([irradianceMap]()
	{
		glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
		glGenerateTextureMipmap(irradianceMap->GetRendererID());
	});

	return Shared<SceneEnvironment>::Create(filepath, envFiltered, irradianceMap);
}

void SceneRenderer::FlushDrawList()
{
	Debug::Assert(!Instance()._sceneInfo.ActiveScene, "");
	Instance()._renderGraph->Execute();

	for (auto& [channel, list] : Instance()._drawContainer)
	{
		list.clear();
	}
}

void SceneRenderer::ResetDrawContainer()
{
	auto& drawContainer = Instance()._drawContainer;
	drawContainer.clear();
	drawContainer.insert(CreatePair(RenderChannel::Main, ArrayList<DrawCommand>{}));
	drawContainer.insert(CreatePair(RenderChannel::Shadow, ArrayList<DrawCommand>{}));
	drawContainer.insert(CreatePair(RenderChannel::Outline, ArrayList<DrawCommand>{}));
}

const Shared<Texture2D>& SceneRenderer::GetFinalColorBuffer()
{
	// return common.CompositePass->GetSpecification().TargetFramebuffer;
	Debug::Assert(false, "Not implemented");
	return nullptr;
}

const RenderPass& SceneRenderer::GetFinalRenderPass()
{
	const auto& output = Instance()._renderGraph->GetFinalRenderPass();
	return output;
}

Uint32 SceneRenderer::GetFinalColorBufferRendererID()
{
	return Instance()._renderGraph->GetOutput()->GetColorAttachmentRendererID();
}

void SceneRenderer::SetFocusPoint(const Vector2f& point)
{
	Instance()._common.FocusPoint = point;
}

void SceneRenderer::SetViewportSize(Uint32 width, Uint32 height)
{
	Instance()._renderGraph->OnViewportResize(width, height);
}

SceneRendererOptions& SceneRenderer::GetOptions()
{
	return Instance()._options;
}

ArrayList<DrawCommand>& SceneRenderer::GetDrawCommands(RenderChannel channel)
{
	return Instance()._drawContainer.at(channel);
}

SceneRendererCommon& SceneRenderer::GetCommon()
{
	return Instance()._common;
}

SceneRendererStats& SceneRenderer::GetStats()
{
	return Instance()._stats;
}

SceneInfo& SceneRenderer::GetSceneInfo()
{
	return Instance()._sceneInfo;
}

ShadowMapData& SceneRenderer::GetShadowMapData()
{
	return Instance()._smData;
}
}

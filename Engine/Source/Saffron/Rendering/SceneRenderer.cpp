#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneEnvironment.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
struct SceneRendererData
{
	const Scene* ActiveScene = nullptr;

	struct SceneInfo
	{
		SceneRendererCamera SceneCamera;

		// Resources
		Shared<MaterialInstance> SkyboxMaterial;
		Shared<SceneEnvironment> SceneEnvironment;
		float SceneEnvironmentIntensity;
		LightEnvironment LightEnvironment;
		Light ActiveLight;

		void Clear()
		{
			SceneCamera = {};
			SkyboxMaterial = nullptr;
			SceneEnvironment = nullptr;
			LightEnvironment = {};
			ActiveLight = {};
		}
	} SceneData;

	Shared<Texture2D> BRDFLUT;
	Shared<Shader> CompositeShader;
	Shared<Shader> BloomBlurShader;
	Shared<Shader> BloomBlendShader;

	Shared<RenderPass> GeoPass;
	Shared<RenderPass> CompositePass;
	Shared<RenderPass> BloomBlurPass[2];
	Shared<RenderPass> BloomBlendPass;

	Shared<Shader> ShadowMapShader;
	Shared<RenderPass> ShadowMapRenderPass[4];
	float ShadowMapSize = 20.0f;
	float LightDistance = 0.1f;
	glm::mat4 LightMatrices[4];
	glm::mat4 LightViewMatrix;
	float CascadeSplitLambda = 0.91f;
	glm::vec4 CascadeSplits;
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

	glm::vec2 FocusPoint = {0.5f, 0.5f};

	RendererID ShadowMapSampler;

	struct DrawCommand
	{
		Shared<Mesh> Mesh;
		Shared<MaterialInstance> Material;
		glm::mat4 Transform;
	};

	std::vector<DrawCommand> DrawList;
	std::vector<DrawCommand> SelectedMeshDrawList;
	std::vector<DrawCommand> ShadowPassDrawList;

	// Grid
	Shared<MaterialInstance> GridMaterial;
	Shared<MaterialInstance> OutlineMaterial;

	SceneRendererOptions Options;
};

struct SceneRendererStats
{
	float ShadowPass = 0.0f;
	float GeometryPass = 0.0f;
	float CompositePass = 0.0f;

	Timer ShadowPassTimer;
	Timer GeometryPassTimer;
	Timer CompositePassTimer;
};


SceneRenderer::SceneRenderer() :
	SingleTon(this),
	_stats(new SceneRendererStats),
	_data(new SceneRendererData)
{
	FramebufferSpecification geoFramebufferSpec;
	geoFramebufferSpec.Attachments = {
		FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth
	};
	geoFramebufferSpec.Samples = 8;
	geoFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPassSpecification geoRenderPassSpec;
	geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
	_data->GeoPass = RenderPass::Create(geoRenderPassSpec);

	FramebufferSpecification compFramebufferSpec;
	compFramebufferSpec.Attachments = {FramebufferTextureFormat::RGBA8};
	compFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPassSpecification compRenderPassSpec;
	compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
	_data->CompositePass = RenderPass::Create(compRenderPassSpec);

	FramebufferSpecification bloomBlurFramebufferSpec;
	bloomBlurFramebufferSpec.Attachments = {FramebufferTextureFormat::RGBA16F};
	bloomBlurFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPassSpecification bloomBlurRenderPassSpec;
	bloomBlurRenderPassSpec.TargetFramebuffer = Framebuffer::Create(bloomBlurFramebufferSpec);
	_data->BloomBlurPass[0] = RenderPass::Create(bloomBlurRenderPassSpec);
	bloomBlurRenderPassSpec.TargetFramebuffer = Framebuffer::Create(bloomBlurFramebufferSpec);
	_data->BloomBlurPass[1] = RenderPass::Create(bloomBlurRenderPassSpec);

	FramebufferSpecification bloomBlendFramebufferSpec;
	bloomBlendFramebufferSpec.Attachments = {FramebufferTextureFormat::RGBA8};
	bloomBlendFramebufferSpec.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

	RenderPassSpecification bloomBlendRenderPassSpec;
	bloomBlendRenderPassSpec.TargetFramebuffer = Framebuffer::Create(bloomBlendFramebufferSpec);
	_data->BloomBlendPass = RenderPass::Create(bloomBlendRenderPassSpec);

	_data->CompositeShader = Shader::Create("SceneComposite");
	_data->BloomBlurShader = Shader::Create("BloomBlur");
	_data->BloomBlendShader = Shader::Create("BloomBlend");
	_data->BRDFLUT = Texture2D::Create("BRDF_LUT.tga");

	// Grid
	auto gridShader = Shader::Create("Grid");
	_data->GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
	_data->GridMaterial->SetFlag(MaterialFlag::TwoSided, true);
	float gridScale = 16.025f, gridSize = 0.025f;
	_data->GridMaterial->Set("u_Scale", gridScale);
	_data->GridMaterial->Set("u_Res", gridSize);

	// Outline
	auto outlineShader = Shader::Create("Outline");
	_data->OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
	_data->OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);

	// Shadow Map
	_data->ShadowMapShader = Shader::Create("ShadowMap");

	FramebufferSpecification shadowMapFramebufferSpec;
	shadowMapFramebufferSpec.Width = 4096;
	shadowMapFramebufferSpec.Height = 4096;
	shadowMapFramebufferSpec.Attachments = {FramebufferTextureFormat::DEPTH32F};
	shadowMapFramebufferSpec.ClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
	shadowMapFramebufferSpec.NoResize = true;

	// 4 cascades
	for (int i = 0; i < 4; i++)
	{
		RenderPassSpecification shadowMapRenderPassSpec;
		shadowMapRenderPassSpec.TargetFramebuffer = Framebuffer::Create(shadowMapFramebufferSpec);
		_data->ShadowMapRenderPass[i] = RenderPass::Create(shadowMapRenderPassSpec);
	}

	Renderer::Submit([this]()
	{
		glGenSamplers(1, &_data->ShadowMapSampler);

		// Setup the shadowmap depth sampler
		glSamplerParameteri(_data->ShadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_data->ShadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_data->ShadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_data->ShadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	});
}

SceneRenderer::~SceneRenderer()
{
	delete _stats;
	delete _data;
}

void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
{
	auto& instData = *Instance()._data;

	instData.GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
	instData.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
}

void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
{
	auto& instData = *Instance()._data;

	SE_CORE_ASSERT(!instData.ActiveScene, "");

	instData.ActiveScene = scene;

	instData.SceneData.SceneCamera = camera;
	instData.SceneData.SkyboxMaterial = scene->GetSkybox().Material;
	instData.SceneData.SceneEnvironment = scene->GetSceneEnvironment();
	instData.SceneData.SceneEnvironmentIntensity = scene->GetSceneEnvironment()->GetIntensity();
	instData.SceneData.ActiveLight = scene->GetLight();
	instData.SceneData.LightEnvironment = scene->GetLightEnvironment();
}

void SceneRenderer::EndScene()
{
	auto& instData = *Instance()._data;

	SE_CORE_ASSERT(instData.ActiveScene, "");

	instData.ActiveScene = nullptr;

	FlushDrawList();
}

void SceneRenderer::SubmitMesh(Shared<Mesh> mesh, const glm::mat4& transform, Shared<MaterialInstance> overrideMaterial)
{
	auto& instData = *Instance()._data;

	// TODO: Culling, sorting, etc.
	instData.DrawList.push_back({mesh, overrideMaterial, transform});
	instData.ShadowPassDrawList.push_back({mesh, overrideMaterial, transform});
}

void SceneRenderer::SubmitSelectedMesh(Shared<Mesh> mesh, const glm::mat4& transform)
{
	auto& instData = *Instance()._data;

	instData.SelectedMeshDrawList.push_back({mesh, nullptr, transform});
	instData.ShadowPassDrawList.push_back({mesh, nullptr, transform});
}

static Shared<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

Shared<SceneEnvironment> SceneRenderer::CreateEnvironmentMap(const Filepath& filepath)
{
	auto& instData = *Instance()._data;

	const uint32_t cubemapSize = 2048;
	const uint32_t irradianceMapSize = 32;

	Shared<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
	if (!equirectangularConversionShader) equirectangularConversionShader = Shader::Create("EquirectangularToCubeMap");
	Shared<Texture2D> envEquirect = Texture2D::Create(filepath);
	SE_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

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
		const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
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

void SceneRenderer::GeometryPass()
{
	auto& instData = *Instance()._data;

	bool outline = instData.SelectedMeshDrawList.size() > 0;

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		});
	}

	Renderer::BeginRenderPass(instData.GeoPass);

	if (outline)
	{
		Renderer::Submit([]()
		{
			glStencilMask(0);
		});
	}

	auto& sceneCamera = instData.SceneData.SceneCamera;

	auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;
	glm::vec3 cameraPosition = glm::inverse(instData.SceneData.SceneCamera.ViewMatrix)[3]; // TODO: Negate instead

	// Skybox
	auto skyboxShader = instData.SceneData.SkyboxMaterial->GetShader();
	instData.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
	instData.SceneData.SkyboxMaterial->Set("u_SkyIntensity", instData.SceneData.SceneEnvironmentIntensity);
	Renderer::SubmitFullscreenQuad(instData.SceneData.SkyboxMaterial);

	float aspectRatio = (float)instData.GeoPass->GetSpecification().TargetFramebuffer->GetWidth() / (float)instData.
		GeoPass->GetSpecification().TargetFramebuffer->GetHeight();
	float frustumSize = 2.0f * sceneCamera.Near * glm::tan(sceneCamera.FOV * 0.5f) * aspectRatio;

	// Render entities
	for (auto& dc : instData.DrawList)
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", sceneCamera.ViewMatrix);
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
		baseMaterial->Set("u_IBLContribution", instData.SceneData.SceneEnvironmentIntensity);

		// SceneEnvironment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", instData.SceneData.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", instData.SceneData.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", instData.BRDFLUT);

		// Set lights (TODO: move to light environment and don't do per mesh)
		auto directionalLight = instData.SceneData.LightEnvironment.DirectionalLights[0];
		baseMaterial->Set("u_DirectionalLights", directionalLight);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if (rd)
		{
			auto reg = rd->GetRegister();

			auto tex = instData.ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->
			                                            GetDepthAttachmentRendererID();
			auto tex1 = instData.ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->
			                                             GetDepthAttachmentRendererID();
			auto tex2 = instData.ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->
			                                             GetDepthAttachmentRendererID();
			auto tex3 = instData.ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->
			                                             GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3]() mutable
			{
				auto& instData = *Instance()._data;
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
	for (auto& dc : instData.SelectedMeshDrawList)
	{
		auto baseMaterial = dc.Mesh->GetMaterial();
		baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		baseMaterial->Set("u_ViewMatrix", sceneCamera.ViewMatrix);
		baseMaterial->Set("u_CameraPosition", cameraPosition);
		baseMaterial->Set("u_CascadeSplits", instData.CascadeSplits);
		baseMaterial->Set("u_ShowCascades", instData.ShowCascades);
		baseMaterial->Set("u_SoftShadows", instData.SoftShadows);
		baseMaterial->Set("u_LightSize", instData.LightSize);
		baseMaterial->Set("u_MaxShadowDistance", instData.MaxShadowDistance);
		baseMaterial->Set("u_ShadowFade", instData.ShadowFade);
		baseMaterial->Set("u_CascadeFading", instData.CascadeFading);
		baseMaterial->Set("u_CascadeTransitionFade", instData.CascadeTransitionFade);
		baseMaterial->Set("u_IBLContribution", instData.SceneData.SceneEnvironmentIntensity);

		// SceneEnvironment (TODO: don't do this per mesh)
		baseMaterial->Set("u_EnvRadianceTex", instData.SceneData.SceneEnvironment->GetRadianceMap());
		baseMaterial->Set("u_EnvIrradianceTex", instData.SceneData.SceneEnvironment->GetIrradianceMap());
		baseMaterial->Set("u_BRDFLUTTexture", instData.BRDFLUT);

		baseMaterial->Set("u_LightMatrixCascade0", instData.LightMatrices[0]);
		baseMaterial->Set("u_LightMatrixCascade1", instData.LightMatrices[1]);
		baseMaterial->Set("u_LightMatrixCascade2", instData.LightMatrices[2]);
		baseMaterial->Set("u_LightMatrixCascade3", instData.LightMatrices[3]);

		// Set lights (TODO: move to light environment and don't do per mesh)
		baseMaterial->Set("u_DirectionalLights", instData.SceneData.LightEnvironment.DirectionalLights[0]);

		auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
		if (rd)
		{
			auto reg = rd->GetRegister();

			auto tex = instData.ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->
			                                            GetDepthAttachmentRendererID();
			auto tex1 = instData.ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->
			                                             GetDepthAttachmentRendererID();
			auto tex2 = instData.ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->
			                                             GetDepthAttachmentRendererID();
			auto tex3 = instData.ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->
			                                             GetDepthAttachmentRendererID();

			Renderer::Submit([reg, tex, tex1, tex2, tex3]() mutable
			{
				auto& instData = *Instance()._data;
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
		                     glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		                     glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
	}

	if (GetOptions().ShowBoundingBoxes)
	{
		//Renderer2D::BeginScene(viewProjection);
		//for (auto& dc : instData.DrawList) Renderer::DrawAABB(dc.Mesh, dc.Transform);
		//Renderer2D::EndScene();
	}

	Renderer::EndRenderPass();
}

void SceneRenderer::CompositePass()
{
	auto& instData = *Instance()._data;

	auto& compositeBuffer = instData.CompositePass->GetSpecification().TargetFramebuffer;

	Renderer::BeginRenderPass(instData.CompositePass);
	instData.CompositeShader->Bind();
	instData.CompositeShader->SetFloat("u_Exposure", instData.SceneData.SceneCamera.Camera.GetExposure());
	instData.CompositeShader->SetInt("u_TextureSamples",
	                                 instData.GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().
	                                          Samples);
	instData.CompositeShader->SetFloat2("u_ViewportSize",
	                                    glm::vec2(compositeBuffer->GetWidth(), compositeBuffer->GetHeight()));
	instData.CompositeShader->SetFloat2("u_FocusPoint", instData.FocusPoint);
	instData.CompositeShader->SetInt("u_TextureSamples",
	                                 instData.GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().
	                                          Samples);
	instData.CompositeShader->SetFloat("u_BloomThreshold", instData.BloomThreshold);
	instData.GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
	Renderer::Submit([instData]()
	{
		glBindTextureUnit(1, instData.GeoPass->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID());
	});
	Renderer::SubmitFullscreenQuad(nullptr);
	Renderer::EndRenderPass();
}

void SceneRenderer::BloomBlurPass()
{
	auto& instData = *Instance()._data;

	int amount = 10;
	int index = 0;

	int horizontalCounter = 0, verticalCounter = 0;
	for (int i = 0; i < amount; i++)
	{
		index = i % 2;
		Renderer::BeginRenderPass(instData.BloomBlurPass[index]);
		instData.BloomBlurShader->Bind();
		instData.BloomBlurShader->SetBool("u_Horizontal", index);
		if (index) horizontalCounter++;
		else verticalCounter++;
		if (i > 0)
		{
			auto fb = instData.BloomBlurPass[1 - index]->GetSpecification().TargetFramebuffer;
			fb->BindTexture();
		}
		else
		{
			auto fb = instData.CompositePass->GetSpecification().TargetFramebuffer;
			auto id = fb->GetColorAttachmentRendererID(1);
			Renderer::Submit([id]()
			{
				glBindTextureUnit(0, id);
			});
		}
		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	// Composite bloom
	{
		Renderer::BeginRenderPass(instData.BloomBlendPass);
		instData.BloomBlendShader->Bind();
		instData.BloomBlendShader->SetFloat("u_Exposure", instData.SceneData.SceneCamera.Camera.GetExposure());
		instData.BloomBlendShader->SetBool("u_EnableBloom", instData.EnableBloom);

		instData.CompositePass->GetSpecification().TargetFramebuffer->BindTexture(0);
		instData.BloomBlurPass[index]->GetSpecification().TargetFramebuffer->BindTexture(1);

		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}
}


struct FrustumBounds
{
	float r, l, b, t, f, n;
};

struct CascadeData
{
	glm::mat4 ViewProj;
	glm::mat4 View;
	float SplitDepth;
};

void SceneRenderer::CalculateCascades(CascadeData* cascades, const glm::vec3& lightDirection)
{
	auto& instData = *Instance()._data;

	FrustumBounds frustumBounds[3];

	auto& sceneCamera = instData.SceneData.SceneCamera;
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

		glm::vec3 frustumCorners[8] = {
			glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f),
		};

		// Project frustum corners into world space
		glm::mat4 invCam = glm::inverse(viewProjection);
		for (uint32_t i = 0; i < 8; i++)
		{
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		glm::vec3 frustumCenter = glm::vec3(0.0f);
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

		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		glm::vec3 lightDir = -lightDirection;
		glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter,
		                                        glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y,
		                                        0.0f + instData.CascadeNearPlaneOffset,
		                                        maxExtents.z - minExtents.z + instData.CascadeFarPlaneOffset);

		// Store split distance and matrix in cascade
		cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
		cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
		cascades[i].View = lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}
}

void SceneRenderer::ShadowMapPass()
{
	auto& instData = *Instance()._data;

	auto& directionalLights = instData.SceneData.LightEnvironment.DirectionalLights;
	if (directionalLights[0].Multiplier == 0.0f || !directionalLights[0].CastShadows)
	{
		for (int i = 0; i < 4; i++)
		{
			// Clear shadow maps
			Renderer::BeginRenderPass(instData.ShadowMapRenderPass[i]);
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

		Renderer::BeginRenderPass(instData.ShadowMapRenderPass[i]);

		glm::mat4 shadowMapVP = cascades[i].ViewProj;
		instData.ShadowMapShader->SetMat4("u_ViewProjection", shadowMapVP);

		static glm::mat4 scaleBiasMatrix = glm::scale(glm::mat4(1.0f), {0.5f, 0.5f, 0.5f}) * glm::translate(
			glm::mat4(1.0f), {1, 1, 1});
		instData.LightMatrices[i] = scaleBiasMatrix * cascades[i].ViewProj;


		// Render entities
		for (auto& dc : instData.ShadowPassDrawList)
		{
			Renderer::SubmitMeshWithShader(dc.Mesh, dc.Transform, instData.ShadowMapShader);
		}

		Renderer::EndRenderPass();
	}
}

void SceneRenderer::FlushDrawList()
{
	auto& instData = *Instance()._data;

	SE_CORE_ASSERT(!instData.ActiveScene, "");

	memset(Instance()._stats, 0, sizeof(SceneRendererStats));

	{
		Renderer::Submit([]()
		{
			Instance()._stats->ShadowPassTimer.Sync();
		});
		ShadowMapPass();
		Renderer::Submit([]
		{
			Instance()._stats->ShadowPass = Instance()._stats->ShadowPassTimer.Mark().sec();
		});
	}
	{
		Renderer::Submit([]()
		{
			Instance()._stats->GeometryPassTimer.Sync();
		});
		GeometryPass();
		Renderer::Submit([]
		{
			Instance()._stats->GeometryPass = Instance()._stats->GeometryPassTimer.Mark().sec();
		});
	}
	{
		Renderer::Submit([]()
		{
			Instance()._stats->CompositePassTimer.Sync();
		});

		CompositePass();
		Renderer::Submit([]
		{
			Instance()._stats->CompositePass = Instance()._stats->CompositePassTimer.Mark().sec();
		});

		//	BloomBlurPass();
	}

	instData.DrawList.clear();
	instData.SelectedMeshDrawList.clear();
	instData.ShadowPassDrawList.clear();
	instData.SceneData.Clear();
}

Shared<Texture2D> SceneRenderer::GetFinalColorBuffer()
{
	// return instData.CompositePass->GetSpecification().TargetFramebuffer;
	SE_CORE_ASSERT(false, "Not implemented");
	return nullptr;
}

Shared<RenderPass> SceneRenderer::GetFinalRenderPass()
{
	return Instance()._data->CompositePass;
}

uint32_t SceneRenderer::GetFinalColorBufferRendererID()
{
	return Instance()._data->CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
}

void SceneRenderer::SetFocusPoint(const glm::vec2& point)
{
	Instance()._data->FocusPoint = point;
}

SceneRendererOptions& SceneRenderer::GetOptions()
{
	return Instance()._data->Options;
}

void SceneRenderer::OnImGuiRender()
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
			static int cascadeIndex = 0;
			auto fb = instData.ShadowMapRenderPass[cascadeIndex]->GetSpecification().TargetFramebuffer;
			auto id = fb->GetDepthAttachmentRendererID();

			float size = ImGui::GetContentRegionAvailWidth();
			// (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
			Gui::BeginPropertyGrid();
			Gui::Property("Cascade Index", cascadeIndex, 0, 3, 1, Gui::PropertyFlag::Slider);
			Gui::EndPropertyGrid();
			ImGui::Image((ImTextureID)id, {size, size}, {0, 1}, {1, 0});
			Gui::EndTreeNode();
		}

		Gui::EndTreeNode();
	}

	if (Gui::BeginTreeNode("Bloom"))
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Bloom", instData.EnableBloom);
		Gui::Property("Bloom threshold", instData.BloomThreshold, 0.05f);
		Gui::EndPropertyGrid();

		auto fb = instData.BloomBlurPass[0]->GetSpecification().TargetFramebuffer;
		auto id = fb->GetColorAttachmentRendererID();

		float size = ImGui::GetContentRegionAvailWidth(); // (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
		float w = size;
		float h = w / ((float)fb->GetWidth() / (float)fb->GetHeight());
		ImGui::Image((ImTextureID)id, {w, h}, {0, 1}, {1, 0});
		Gui::EndTreeNode();
	}


	ImGui::End();
}
}

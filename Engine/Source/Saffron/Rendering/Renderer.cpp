#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Rendering/Material.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RendererAPI.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
struct RendererData
{
	Shared<RenderPass> m_ActiveRenderPass;
	RenderCommandQueue m_CommandQueue;

	Shared<VertexBuffer> m_FullscreenQuadVertexBuffer;
	Shared<IndexBuffer> m_FullscreenQuadIndexBuffer;
	Shared<Pipeline> m_FullscreenQuadPipeline;

	static constexpr Uint32 MaxLines = 10000;
	static constexpr Uint32 MaxLineVertices = MaxLines * 2;
	static constexpr Uint32 MaxLineIndices = MaxLines * 2;
	Shared<Pipeline> LinePipeline;
	Shared<VertexBuffer> LineVertexBuffer;
	Shared<IndexBuffer> LineIndexBuffer;
};


RendererApiType RendererAPI::m_sCurrentAPI = RendererApiType::OpenGL;

Renderer::Renderer() :
	SingleTon(this),
	_data(new RendererData)
{
	Submit([]() { RendererAPI::Init(); });

	auto staticShader = Shader::Create("SaffronPBR_Static");
	auto animShader = Shader::Create("SaffronPBR_Anim");

	// Create fullscreen quad
	float x = -1;
	float y = -1;
	float width = 2, height = 2;
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	auto* data = new QuadVertex[4];

	data[0].Position = glm::vec3(x, y, 0.1f);
	data[0].TexCoord = glm::vec2(0, 0);

	data[1].Position = glm::vec3(x + width, y, 0.1f);
	data[1].TexCoord = glm::vec2(1, 0);

	data[2].Position = glm::vec3(x + width, y + height, 0.1f);
	data[2].TexCoord = glm::vec2(1, 1);

	data[3].Position = glm::vec3(x, y + height, 0.1f);
	data[3].TexCoord = glm::vec2(0, 1);

	PipelineSpecification pipelineSpecification;
	pipelineSpecification.Layout = {{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}};
	_data->m_FullscreenQuadPipeline = Pipeline::Create(pipelineSpecification);

	_data->m_FullscreenQuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
	uint32_t indices[6] = {0, 1, 2, 2, 3, 0,};
	_data->m_FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));
}

Renderer::~Renderer()
{
	delete _data;
}

void Renderer::OnGuiRender()
{
	const Time ts = Global::Timer::GetStep();

	static Time CachedFrametime = Time::Zero();
	static Time AverageFrameTimeCounter = Time::Zero();
	static Uint32 m_CachedFrameCounter = 0;
	m_CachedFrameCounter++;
	if ((AverageFrameTimeCounter += ts).sec() > 1.0f)
	{
		CachedFrametime = AverageFrameTimeCounter / static_cast<float>(m_CachedFrameCounter);
		AverageFrameTimeCounter = Time::Zero();
		m_CachedFrameCounter = 0;
	}

	static Time SavedTS = Time::Zero();
	static Time Counter = Time::Zero();
	if ((Counter += ts).sec() > 0.1f)
	{
		SavedTS = ts;
		Counter = Time::Zero();
	}

	const auto& caps = RendererAPI::GetCapabilities();

	ImGui::Begin("Renderer");
	ImGui::Text("Vendor: %s", caps.Vendor.c_str());
	ImGui::Text("Renderer: %s", caps.Renderer.c_str());
	ImGui::Text("Version: %s", caps.Version.c_str());
	ImGui::Text("Frame Time: %.2f ms  (%.0f FPS)\n", SavedTS.ms(), 1.0f / SavedTS.sec());
	ImGui::Text("Average: ");
	ImGui::Text("Frame Time: %.2f ms  (%.0f FPS)\n", CachedFrametime.ms(), 1.0f / CachedFrametime.sec());
	ImGui::End();
}

void Renderer::Clear()
{
	Submit([]()
	{
		RendererAPI::Clear(0.0f, 0.0f, 0.0f, 1.0f);
	});
}

void Renderer::Clear(float r, float g, float b, float a)
{
	Submit([=]()
	{
		RendererAPI::Clear(r, g, b, a);
	});
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
}

void Renderer::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
{
	Submit([=]()
	{
		RendererAPI::DrawIndexed(count, type, depthTest);
	});
}

void Renderer::SetLineThickness(float thickness)
{
	Submit([=]()
	{
		RendererAPI::SetLineThickness(thickness);
	});
}

void Renderer::ClearMagenta()
{
	Clear(1, 0, 1);
}

void Renderer::BeginRenderPass(Shared<RenderPass> renderPass, bool clear)
{
	SE_CORE_ASSERT(renderPass, "Render pass cannot be null!");

	// TODO: Convert all of this into a render command buffer
	Instance()._data->m_ActiveRenderPass = renderPass;

	renderPass->GetSpecification().TargetFramebuffer->Bind();
	if (clear)
	{
		const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
		Submit([=]()
		{
			RendererAPI::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		});
	}
}

void Renderer::EndRenderPass()
{
	auto& instData = *Instance()._data;
	SE_CORE_ASSERT(instData.m_ActiveRenderPass,
	               "No active render pass! Have you called Renderer::EndRenderPass twice?");
	instData.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
	instData.m_ActiveRenderPass = nullptr;
}

void Renderer::SubmitQuad(Shared<MaterialInstance> material, const glm::mat4& transform)
{
	bool depthTest = true;
	bool cullFace = true;
	if (material)
	{
		material->Bind();
		depthTest = material->GetFlag(MaterialFlag::DepthTest);
		cullFace = !material->GetFlag(MaterialFlag::TwoSided);

		auto shader = material->GetShader();
		shader->SetMat4("u_Transform", transform);
	}

	if (cullFace)Submit([]() { glEnable(GL_CULL_FACE); });
	else Submit([]() { glDisable(GL_CULL_FACE); });

	auto& instData = *Instance()._data;
	instData.m_FullscreenQuadVertexBuffer->Bind();
	instData.m_FullscreenQuadPipeline->Bind();
	instData.m_FullscreenQuadIndexBuffer->Bind();
	DrawIndexed(6, PrimitiveType::Triangles, depthTest);
}

void Renderer::SubmitMesh(Shared<Mesh> mesh, const glm::mat4& transform, Shared<MaterialInstance> overrideMaterial)
{
	// auto material = overrideMaterial ? overrideMaterial : mesh->GetMaterialInstance();
	// auto shader = material->GetShader();
	// TODO: Sort this out
	mesh->m_VertexBuffer->Bind();
	mesh->m_Pipeline->Bind();
	mesh->m_IndexBuffer->Bind();

	auto& materials = mesh->GetMaterials();
	for (Submesh& submesh : mesh->m_Submeshes)
	{
		// Material
		auto material = overrideMaterial ? overrideMaterial : materials[submesh.MaterialIndex];
		auto shader = material->GetShader();
		material->Bind();

		if (mesh->m_IsAnimated)
		{
			for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
			{
				std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
				mesh->m_MeshShader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
			}
		}
		shader->SetMat4("u_Transform", transform * submesh.Transform);

		Submit([submesh, material]()
		{
			if (material->GetFlag(MaterialFlag::DepthTest))
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			if (!material->GetFlag(MaterialFlag::TwoSided)) Submit([]() { glEnable(GL_CULL_FACE); });
			else Submit([]() { glDisable(GL_CULL_FACE); });

			glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT,
			                         (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
		});
	}
}

RenderCommandQueue& Renderer::GetRenderCommandQueue()
{
	return Instance()._data->m_CommandQueue;
}

void Renderer::WaitAndRender()
{
	Instance()._data->m_CommandQueue.Execute();
}

void Renderer::SubmitFullscreenQuad(Shared<MaterialInstance> material)
{
	bool depthTest = true;
	bool cullFace = true;
	if (material)
	{
		material->Bind();
		depthTest = material->GetFlag(MaterialFlag::DepthTest);
		cullFace = !material->GetFlag(MaterialFlag::TwoSided);
	}

	auto& instData = *Instance()._data;
	instData.m_FullscreenQuadVertexBuffer->Bind();
	instData.m_FullscreenQuadPipeline->Bind();
	instData.m_FullscreenQuadIndexBuffer->Bind();

	if (cullFace)Submit([]() { glEnable(GL_CULL_FACE); });
	else Submit([]() { glDisable(GL_CULL_FACE); });

	DrawIndexed(6, PrimitiveType::Triangles, depthTest);
}

void Renderer::SubmitMeshWithShader(Shared<Mesh> mesh, const glm::mat4& transform, Shared<Shader> shader)
{
	mesh->m_VertexBuffer->Bind();
	mesh->m_Pipeline->Bind();
	mesh->m_IndexBuffer->Bind();

	for (Submesh& submesh : mesh->m_Submeshes)
	{
		shader->SetMat4("u_Transform", transform * submesh.Transform);

		Submit([submesh]()
		{
			glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT,
			                         (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
		});
	}
}

void Renderer::DrawAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color)
{
	glm::vec4 min = {aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f};
	glm::vec4 max = {aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f};

	glm::vec4 corners[8] = {
		transform * glm::vec4{aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f},
		transform * glm::vec4{aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f},
		transform * glm::vec4{aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f},
		transform * glm::vec4{aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f},
		transform * glm::vec4{aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f},
		transform * glm::vec4{aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f},
		transform * glm::vec4{aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f},
		transform * glm::vec4{aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f}
	};

	//for (uint32_t i = 0; i < 4; i++) Renderer2D::DrawLine(corners[i], corners[(i + 1) % 4], color);

	//for (uint32_t i = 0; i < 4; i++) Renderer2D::DrawLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

	//for (uint32_t i = 0; i < 4; i++) Renderer2D::DrawLine(corners[i], corners[i + 4], color);
}

void Renderer::DrawAABB(Shared<Mesh> mesh, const glm::mat4& transform, const glm::vec4& color)
{
	for (Submesh& submesh : mesh->m_Submeshes)
	{
		auto& aabb = submesh.BoundingBox;
		auto aabbTransform = transform * submesh.Transform;
		DrawAABB(aabb, aabbTransform);
	}
}
}

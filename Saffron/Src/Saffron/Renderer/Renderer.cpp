#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Renderer2D.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Renderer/Shader.h"

namespace Se
{

RendererAPI::Type RendererAPI::m_sCurrentAPI = Type::OpenGL;

struct RendererData
{
	Ref<RenderPass> m_ActiveRenderPass;
	RenderCommandQueue m_CommandQueue;
	Ref<ShaderLibrary> m_ShaderLibrary;

	Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
	Ref<IndexBuffer> m_FullscreenQuadIndexBuffer;
	Ref<Pipeline> m_FullscreenQuadPipeline;
};

static RendererData s_Data;

void Renderer::Init()
{
	s_Data.m_ShaderLibrary = Ref<ShaderLibrary>::Create();
	Submit([]() { RendererAPI::Init(); });

	GetShaderLibrary()->Load("Assets/Shaders/SaffronPBR_Static.glsl");
	GetShaderLibrary()->Load("Assets/Shaders/SaffronPBR_Anim.glsl");

	SceneRenderer::Init();

	// Create fullscreen quad
	const float x = -1, y = -1, width = 2, height = 2;
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	auto *data = new QuadVertex[4];

	data[0].Position = glm::vec3(x, y, 0.1f);
	data[0].TexCoord = glm::vec2(0, 0);

	data[1].Position = glm::vec3(x + width, y, 0.1f);
	data[1].TexCoord = glm::vec2(1, 0);

	data[2].Position = glm::vec3(x + width, y + height, 0.1f);
	data[2].TexCoord = glm::vec2(1, 1);

	data[3].Position = glm::vec3(x, y + height, 0.1f);
	data[3].TexCoord = glm::vec2(0, 1);

	Pipeline::Specification Specification;
	Specification.Layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" }
	};
	s_Data.m_FullscreenQuadPipeline = Pipeline::Create(Specification);

	s_Data.m_FullscreenQuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
	Uint32 indices[6] = { 0, 1, 2, 2, 3, 0, };
	s_Data.m_FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(Uint32));

	Renderer2D::Init();
}

void Renderer::OnImGuiRender()
{


	const Time ts = GlobalTimer::GetStep();

	static Time CachedFrametime = Time::Zero();
	static Time AverageFrameTimeCounter = Time::Zero();
	static Uint32 m_CachedFrameCounter = 0;
	m_CachedFrameCounter++;
	if ( (AverageFrameTimeCounter += ts).sec() > 1.0f )
	{
		CachedFrametime = AverageFrameTimeCounter / static_cast<float>(m_CachedFrameCounter);
		AverageFrameTimeCounter = Time::Zero();
		m_CachedFrameCounter = 0;
	}

	static Time SavedTS = Time::Zero();
	static Time Counter = Time::Zero();
	if ( (Counter += ts).sec() > 0.1f )
	{
		SavedTS = ts;
		Counter = Time::Zero();
	}

	const auto &caps = RendererAPI::GetCapabilities();
	const auto &stats = Renderer2D::GetStats();

	ImGui::Begin("Renderer");
	ImGui::Text("Vendor: %s", caps.Vendor.c_str());
	ImGui::Text("Renderer: %s", caps.Renderer.c_str());
	ImGui::Text("Version: %s", caps.Version.c_str());
	ImGui::Text("Frame Time: %.2f ms  (%.0f FPS)\n", SavedTS.ms(), 1.0f / SavedTS.sec());
	ImGui::Text("Average: ");
	ImGui::Text("Frame Time: %.2f ms  (%.0f FPS)\n", CachedFrametime.ms(), 1.0f / CachedFrametime.sec());
	ImGui::Text("\n");
	ImGui::Text("2D Renderering Statistics: ");
	ImGui::Text("Draw Calls: %u", stats.DrawCalls);
	ImGui::Text("Line Calls: %u", stats.LineCount);
	ImGui::Text("Quad Count: %u", stats.QuadCount);
	ImGui::Text("Total Index Count: %u", stats.GetTotalIndexCount());
	ImGui::Text("Total Vertex Count: %u", stats.GetTotalVertexCount());
	ImGui::End();
}

void Renderer::DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest)
{
	Submit([=]() { RendererAPI::DrawIndexed(count, type, depthTest); });
}

void Renderer::Clear()
{
	Submit([]() { RendererAPI::Clear(0.0f, 0.0f, 0.0f, 1.0f); });
}

void Renderer::Clear(float r, float g, float b, float a)
{
	Submit([=]() { RendererAPI::Clear(r, g, b, a); });
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
}

void Renderer::SetLineThickness(float thickness)
{
	Submit([=]() { RendererAPI::SetLineThickness(thickness); });
}

void Renderer::ClearMagenta()
{
	Clear(1, 0, 1);
}

Ref<ShaderLibrary> Renderer::GetShaderLibrary()
{
	return s_Data.m_ShaderLibrary;
}

void Renderer::WaitAndRender()
{
	s_Data.m_CommandQueue.Execute();
}

void Renderer::BeginRenderPass(Ref<RenderPass> renderPass, bool clear)
{
	SE_CORE_ASSERT(renderPass, "Render pass cannot be null!");

	// TODO: Convert all of this into a render command buffer
	s_Data.m_ActiveRenderPass = renderPass;

	renderPass->GetSpecification().TargetFramebuffer->Bind();
	if ( clear )
	{
		const glm::vec4 &clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
		Submit([=]() { RendererAPI::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a); });
	}
}

void Renderer::EndRenderPass()
{
	SE_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
	s_Data.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
	s_Data.m_ActiveRenderPass = nullptr;
}

void Renderer::SubmitQuad(Ref<MaterialInstance> material, const glm::mat4 &transform)
{
	bool depthTest = true;
	if ( material )
	{
		material->Bind();
		depthTest = material->GetFlag(Material::Flag::DepthTest);

		auto shader = material->GetShader();
		shader->SetMat4("u_Transform", transform);
	}

	s_Data.m_FullscreenQuadVertexBuffer->Bind();
	s_Data.m_FullscreenQuadPipeline->Bind();
	s_Data.m_FullscreenQuadIndexBuffer->Bind();
	DrawIndexed(6, PrimitiveType::Triangles, depthTest);
}

void Renderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4 &transform, Ref<MaterialInstance> overrideMaterial)
{
	// auto material = overrideMaterial ? overrideMaterial : mesh->GetMaterialInstance();
	// auto shader = material->GetShader();
	// TODO: Sort this out
	mesh->m_VertexBuffer->Bind();
	mesh->m_Pipeline->Bind();
	mesh->m_IndexBuffer->Bind();

	auto materials = mesh->GetMaterials();
	for ( Submesh &submesh : mesh->m_Submeshes )
	{
		// Material
		auto material = overrideMaterial ? overrideMaterial : materials[submesh.MaterialIndex];
		auto shader = material->GetShader();
		material->Bind();

		if ( mesh->m_IsAnimated )
		{
			for ( size_t i = 0; i < mesh->m_BoneTransforms.size(); i++ )
			{
				std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
				mesh->m_MeshShader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
			}
		}
		shader->SetMat4("u_Transform", transform * submesh.Transform);

		Submit([submesh, material]() {
			if ( material->GetFlag(Material::Flag::DepthTest) )
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, reinterpret_cast<void *>(sizeof(Uint32) * submesh.BaseIndex), submesh.BaseVertex);
			   });
	}
}

void Renderer::DrawAABB(Ref<Mesh> mesh, const glm::mat4 &transform, const glm::vec4 &color)
{
	for ( Submesh &submesh : mesh->m_Submeshes )
	{
		auto &aabb = submesh.BoundingBox;
		auto aabbTransform = transform * submesh.Transform;
		DrawAABB(aabb, aabbTransform);
	}
}

void Renderer::DrawAABB(const AABB &aabb, const glm::mat4 &transform, const glm::vec4 &color)
{
	glm::vec4 min = { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f };
	glm::vec4 max = { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f };

	glm::vec4 corners[8] =
	{
		transform * glm::vec4 { aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f },
		transform * glm::vec4 { aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f },
		transform * glm::vec4 { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f },
		transform * glm::vec4 { aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f },

		transform * glm::vec4 { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f },
		transform * glm::vec4 { aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f },
		transform * glm::vec4 { aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f },
		transform * glm::vec4 { aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f }
	};

	for ( Uint32 i = 0; i < 4; i++ )
		Renderer2D::DrawLine(corners[i], corners[(i + 1) % 4], color);

	for ( Uint32 i = 0; i < 4; i++ )
		Renderer2D::DrawLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

	for ( Uint32 i = 0; i < 4; i++ )
		Renderer2D::DrawLine(corners[i], corners[i + 4], color);
}

RenderCommandQueue &Renderer::GetRenderCommandQueue()
{
	return s_Data.m_CommandQueue;
}

}
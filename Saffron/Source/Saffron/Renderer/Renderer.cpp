#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Renderer/Shader.h"

namespace Se
{

struct RendererData
{
	std::shared_ptr<RenderPass> m_ActiveRenderPass;
	RenderCommandQueue m_CommandQueue;

	std::shared_ptr<VertexBuffer> m_FullscreenQuadVertexBuffer;
	std::shared_ptr<IndexBuffer> m_FullscreenQuadIndexBuffer;
	std::shared_ptr<Pipeline> m_FullscreenQuadPipeline;

	static constexpr Uint32 MaxLines = 10000;
	static constexpr Uint32 MaxLineVertices = MaxLines * 2;
	static constexpr Uint32 MaxLineIndices = MaxLines * 2;
	std::shared_ptr<Pipeline> LinePipeline;
	std::shared_ptr<VertexBuffer> LineVertexBuffer;
	std::shared_ptr<IndexBuffer> LineIndexBuffer;
};


RendererAPI::Type RendererAPI::m_sCurrentAPI = Type::OpenGL;
RendererData Renderer::s_Data;

void Renderer::Init()
{
	Submit([]() { RendererAPI::Init(); });

	Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/SaffronPBR_Static.glsl" });
	Factory::Create<Shader>(Filepath{ "Resources/Assets/Shaders/SaffronPBR_Anim.glsl" });

	SceneRenderer::Init();

	// Create fullscreen quad
	{
		const float x = -1, y = -1, width = 2, height = 2;
		struct QuadVertex
		{
			Vector3f Position;
			Vector2f TexCoord;
		};

		auto *data = new QuadVertex[4];

		data[0].Position = Vector3f(x, y, 0.1f);
		data[0].TexCoord = Vector2f(0, 0);

		data[1].Position = Vector3f(x + width, y, 0.1f);
		data[1].TexCoord = Vector2f(1, 0);

		data[2].Position = Vector3f(x + width, y + height, 0.1f);
		data[2].TexCoord = Vector2f(1, 1);

		data[3].Position = Vector3f(x, y + height, 0.1f);
		data[3].TexCoord = Vector2f(0, 1);

		Pipeline::Specification Specification;
		Specification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		s_Data.m_FullscreenQuadPipeline = Factory::Create<Pipeline>(Specification);

		s_Data.m_FullscreenQuadVertexBuffer = Factory::Create<VertexBuffer>(data, 4 * sizeof(QuadVertex));
		Uint32 indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data.m_FullscreenQuadIndexBuffer = Factory::Create<IndexBuffer>(indices, 6 * sizeof(Uint32));

	}

	// Lines
	{
		Pipeline::Specification Specification;
		Specification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		s_Data.LinePipeline = Factory::Create<Pipeline>(Specification);

		s_Data.LineVertexBuffer = Factory::Create<VertexBuffer>(RendererData::MaxLineVertices * sizeof(LineVertex));
		auto *lineIndices = new Uint32[RendererData::MaxLineIndices];
		for ( Uint32 i = 0; i < RendererData::MaxLineIndices; i++ )
			lineIndices[i] = i;
		s_Data.LineIndexBuffer = Factory::Create<IndexBuffer>(lineIndices, RendererData::MaxLineIndices);
		delete[] lineIndices;
	}
}

void Renderer::OnGuiRender()
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

	ImGui::Begin("Renderer");
	ImGui::Text("Vendor: %s", caps.Vendor.c_str());
	ImGui::Text("Renderer: %s", caps.Renderer.c_str());
	ImGui::Text("Version: %s", caps.Version.c_str());
	ImGui::Text("Frame Time: %.2f ms  (%.0f FPS)\n", SavedTS.ms(), 1.0f / SavedTS.sec());
	ImGui::Text("Average: ");
	ImGui::Text("Frame Time: %.2f ms  (%.0f FPS)\n", CachedFrametime.ms(), 1.0f / CachedFrametime.sec());
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

void Renderer::Submit(const RenderCommand &func)
{
	s_Data.m_CommandQueue.Submit(func);
}

void Renderer::Execute()
{
	s_Data.m_CommandQueue.Execute();
}

void Renderer::BeginRenderPass(std::shared_ptr<RenderPass> renderPass, bool clear)
{
	SE_CORE_ASSERT(renderPass, "Render pass cannot be null!");

	// TODO: Convert all of this into a render command buffer
	s_Data.m_ActiveRenderPass = renderPass;

	renderPass->GetSpecification().TargetFramebuffer->Bind();
	if ( clear )
	{
		const Vector4f &clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
		Submit([=]() { RendererAPI::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a); });
	}
}

void Renderer::EndRenderPass()
{
	SE_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
	s_Data.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
	s_Data.m_ActiveRenderPass = nullptr;
}

void Renderer::SubmitQuad(std::shared_ptr<MaterialInstance> material, const Matrix4f &transform)
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

void Renderer::SubmitLines(LineVertex *vertices, Uint32 count, std::shared_ptr<Shader> shader)
{
	const auto dataSize = static_cast<Uint32>(sizeof(LineVertex) * count);
	if ( dataSize )
	{
		SetLineThickness(2.0f);

		shader->Bind();

		s_Data.LineVertexBuffer->SetData(vertices, dataSize);

		s_Data.LineVertexBuffer->Bind();
		s_Data.LinePipeline->Bind();
		s_Data.LineIndexBuffer->Bind();

		DrawIndexed(count, PrimitiveType::Lines, true);
	}
}

void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const Matrix4f &transform, std::shared_ptr<MaterialInstance> overrideMaterial)
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
				String uniformName = String("u_BoneTransforms[") + std::to_string(i) + String("]");
				mesh->m_MeshShader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
			}
		}

		shader->SetMat4("u_Transform", transform * mesh->GetLocalTransform() * submesh.Transform);

		Submit([submesh, material]()
			   {
				   material->GetFlag(Material::Flag::DepthTest) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
				   glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, reinterpret_cast<void *>(sizeof(Uint32) * submesh.BaseIndex), submesh.BaseVertex);
			   });
	}
}

void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const Matrix4f &transform, std::shared_ptr<Shader> shader)
{
	mesh->m_VertexBuffer->Bind();
	mesh->m_Pipeline->Bind();
	mesh->m_IndexBuffer->Bind();

	for ( Submesh &submesh : mesh->m_Submeshes )
	{
		shader->SetMat4("u_Transform", transform * submesh.Transform);

		Submit([submesh]()
			   {
				   glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, reinterpret_cast<void *>(sizeof(Uint32) * submesh.BaseIndex), submesh.BaseVertex);
			   });
	}
}

RenderCommandQueue &Renderer::GetRenderCommandQueue()
{
	return s_Data.m_CommandQueue;
}

}
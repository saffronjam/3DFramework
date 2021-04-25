#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Rendering/Material.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RendererApi.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
struct RendererData
{
	Shared<Framebuffer> _activeFramebuffer;
	RenderCommandQueue _commandQueue;

	Shared<VertexBuffer> _fullscreenQuadVertexBuffer;
	Shared<IndexBuffer> _fullscreenQuadIndexBuffer;
	Shared<Pipeline> _fullscreenQuadPipeline;

	static constexpr uint MaxLines = 10000;
	static constexpr uint MaxLineVertices = MaxLines * 2;
	static constexpr uint MaxLineIndices = MaxLines * 2;
	Shared<Pipeline> LinePipeline;
	Shared<VertexBuffer> LineVertexBuffer;
	Shared<IndexBuffer> LineIndexBuffer;
};


RendererApiType RendererApi::_sCurrentAPI = RendererApiType::OpenGL;

Renderer::Renderer() :
	SingleTon(this),
	_data(new RendererData)
{
	Submit([]() { RendererApi::Init(); });

	auto staticShader = Shader::Create("SaffronPBR_Static");
	auto animShader = Shader::Create("SaffronPBR_Anim");

	// Create fullscreen quad
	float x = -1;
	float y = -1;
	float width = 2, height = 2;
	struct QuadVertex
	{
		Vector3 Position;
		Vector2 TexCoord;
	};

	auto* data = new QuadVertex[4];

	data[0].Position = Vector3(x, y, 0.1f);
	data[0].TexCoord = Vector2(0, 0);

	data[1].Position = Vector3(x + width, y, 0.1f);
	data[1].TexCoord = Vector2(1, 0);

	data[2].Position = Vector3(x + width, y + height, 0.1f);
	data[2].TexCoord = Vector2(1, 1);

	data[3].Position = Vector3(x, y + height, 0.1f);
	data[3].TexCoord = Vector2(0, 1);

	PipelineSpecification pipelineSpecification;
	pipelineSpecification.Layout = {{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}};
	_data->_fullscreenQuadPipeline = Pipeline::Create(pipelineSpecification);

	_data->_fullscreenQuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
	uint32_t indices[6] = {0, 1, 2, 2, 3, 0,};
	_data->_fullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));
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
	static uint _cachedFrameCounter = 0;
	_cachedFrameCounter++;
	if ((AverageFrameTimeCounter += ts).sec() > 1.0f)
	{
		CachedFrametime = AverageFrameTimeCounter / static_cast<float>(_cachedFrameCounter);
		AverageFrameTimeCounter = Time::Zero();
		_cachedFrameCounter = 0;
	}

	static Time SavedTS = Time::Zero();
	static Time Counter = Time::Zero();
	if ((Counter += ts).sec() > 0.1f)
	{
		SavedTS = ts;
		Counter = Time::Zero();
	}

	const auto& caps = RendererApi::GetCapabilities();

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
		RendererApi::Clear(0.0f, 0.0f, 0.0f, 1.0f);
	});
}

void Renderer::Clear(float r, float g, float b, float a)
{
	Submit([=]()
	{
		RendererApi::Clear(r, g, b, a);
	});
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
}

void Renderer::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
{
	Submit([=]()
	{
		RendererApi::DrawIndexed(count, type, depthTest);
	});
}

void Renderer::SetLineThickness(float thickness)
{
	Submit([=]()
	{
		RendererApi::SetLineThickness(thickness);
	});
}

void Renderer::ClearMagenta()
{
	Clear(1, 0, 1);
}

void Renderer::Begin(Shared<Framebuffer> framebuffer, bool clear)
{
	Debug::Assert(framebuffer, "Framebuffer cannot be null!");

	// TODO: Convert all of this into a render command buffer
	Instance()._data->_activeFramebuffer = framebuffer;

	framebuffer->Bind();
	if (clear)
	{
		const Vector4 clearColor = framebuffer->GetSpecification().ClearColor;
		Submit([clearColor]()
		{
			RendererApi::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		});
	}
}

void Renderer::End()
{
	auto& instData = *Instance()._data;
	Debug::Assert(instData._activeFramebuffer,
	               "No active render pass! Have you called Renderer::EndRenderPass twice?");
	instData._activeFramebuffer->Unbind();
	instData._activeFramebuffer = nullptr;
}

void Renderer::SubmitQuad(Shared<MaterialInstance> material, const Matrix4& transform)
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
	instData._fullscreenQuadVertexBuffer->Bind();
	instData._fullscreenQuadPipeline->Bind();
	instData._fullscreenQuadIndexBuffer->Bind();
	DrawIndexed(6, PrimitiveType::Triangles, depthTest);
}

void Renderer::SubmitMesh(Shared<Mesh> mesh, const Matrix4& transform, Shared<MaterialInstance> overrideMaterial)
{
	// auto material = overrideMaterial ? overrideMaterial : mesh->GetMaterialInstance();
	// auto shader = material->GetShader();
	// TODO: Sort this out
	mesh->_vertexBuffer->Bind();
	mesh->_pipeline->Bind();
	mesh->_indexBuffer->Bind();

	auto& materials = mesh->GetMaterials();
	for (Submesh& submesh : mesh->_submeshes)
	{
		// Material
		auto material = overrideMaterial ? overrideMaterial : materials[submesh.MaterialIndex];
		auto shader = material->GetShader();
		material->Bind();

		if (mesh->_isAnimated)
		{
			for (size_t i = 0; i < mesh->_boneTransforms.size(); i++)
			{
				std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
				mesh->_meshShader->SetMat4(uniformName, mesh->_boneTransforms[i]);
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
	return Instance()._data->_commandQueue;
}

void Renderer::WaitAndRender()
{
	Instance()._data->_commandQueue.Execute();
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
	instData._fullscreenQuadVertexBuffer->Bind();
	instData._fullscreenQuadPipeline->Bind();
	instData._fullscreenQuadIndexBuffer->Bind();

	if (cullFace)Submit([]() { glEnable(GL_CULL_FACE); });
	else Submit([]() { glDisable(GL_CULL_FACE); });

	DrawIndexed(6, PrimitiveType::Triangles, depthTest);
}

void Renderer::SubmitMeshWithShader(Shared<Mesh> mesh, const Matrix4& transform, Shared<Shader> shader)
{
	mesh->_vertexBuffer->Bind();
	mesh->_pipeline->Bind();
	mesh->_indexBuffer->Bind();

	for (Submesh& submesh : mesh->_submeshes)
	{
		shader->SetMat4("u_Transform", transform * submesh.Transform);

		Submit([submesh]()
		{
			glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT,
			                         (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
		});
	}
}

void Renderer::DrawAABB(const AABB& aabb, const Matrix4& transform, const Vector4& color)
{
	Vector4 min = {aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f};
	Vector4 max = {aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f};

	Vector4 corners[8] = {
		transform * Vector4{aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f},
		transform * Vector4{aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f},
		transform * Vector4{aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f},
		transform * Vector4{aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f},
		transform * Vector4{aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f},
		transform * Vector4{aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f},
		transform * Vector4{aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f},
		transform * Vector4{aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f}
	};

	//for (uint32_t i = 0; i < 4; i++) Renderer2D::DrawLine(corners[i], corners[(i + 1) % 4], color);

	//for (uint32_t i = 0; i < 4; i++) Renderer2D::DrawLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

	//for (uint32_t i = 0; i < 4; i++) Renderer2D::DrawLine(corners[i], corners[i + 4], color);
}

void Renderer::DrawAABB(Shared<Mesh> mesh, const Matrix4& transform, const Vector4& color)
{
	for (Submesh& submesh : mesh->_submeshes)
	{
		auto& aabb = submesh.BoundingBox;
		auto aabbTransform = transform * submesh.Transform;
		DrawAABB(aabb, aabbTransform);
	}
}
}

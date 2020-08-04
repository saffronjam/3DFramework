#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Renderer2D.h"

namespace Se
{

Scope<Renderer::SceneData> Renderer::m_sSceneData = CreateScope<SceneData>();

void Renderer::Init()
{
	//SE_PROFILE_FUNCTION();

	RenderCommand::Init();
	Renderer2D::Init();
}

void Renderer::Shutdown()
{
	Renderer2D::Shutdown();
}

void Renderer::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizeEvent>(&OnWindowResize);
}

void Renderer::BeginScene(Camera2D &camera)
{
	m_sSceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::BeginScene(Camera3D &camera)
{
	m_sSceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, const glm::mat4 &transform)
{
	shader->Bind();
	shader->SetMat4("u_ViewProjection", m_sSceneData->ViewProjectionMatrix);
	shader->SetMat4("u_Transform", transform);

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}

RendererAPI::API Renderer::GetAPI()
{
	return RendererAPI::GetAPI();
}

void Renderer::OnWindowResize(const WindowResizeEvent &event)
{
	RenderCommand::SetViewport(0, 0, event.GetWidth(), event.GetHeight());
}

}

#include "Saffron/SaffronPCH.h"
#include "RenderCommand.h"

namespace Se
{

Scope<RendererAPI> RenderCommand::m_sRendererAPI = RendererAPI::Create();

void RenderCommand::Init()
{
	m_sRendererAPI->Init();
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	m_sRendererAPI->SetViewport(x, y, width, height);
}

void RenderCommand::SetClearColor(const glm::vec4 &color)
{
	m_sRendererAPI->SetClearColor(color);
}

void RenderCommand::Clear()
{
	m_sRendererAPI->Clear();
}

void RenderCommand::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count)
{
	m_sRendererAPI->DrawIndexed(vertexArray, count);
}
}

#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
static GLenum OpenGLUsage(VertexBufferUsage usage)
{
	switch (usage)
	{
	case VertexBufferUsage::Static: return GL_STATIC_DRAW;
	case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
	}
	SE_CORE_ASSERT(false, "Unknown vertex buffer usage");
	return 0;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage) :
	m_Size(size),
	m_Usage(usage)
{
	m_LocalData = Buffer::Copy(data, size);
	Shared<OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data(),
		                  OpenGLUsage(instance->m_Usage));
	});
}

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage) :
	m_Size(size),
	m_Usage(usage)
{
	Shared<OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, OpenGLUsage(instance->m_Usage));
	});
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteBuffers(1, &rendererID);
	});
}

void OpenGLVertexBuffer::Bind() const
{
	Shared<const OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance]()
	{
		glBindBuffer(GL_ARRAY_BUFFER, instance->m_RendererID);
	});
}

void OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	m_LocalData = Buffer::Copy(data, size);
	m_Size = size;
	Shared<OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance, offset]()
	{
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
	});
}

void OpenGLVertexBuffer::SetData(const Buffer& buffer, uint32_t offset)
{
	SetData(buffer.Data(), buffer.Size(), offset);
}

const VertexBufferLayout& OpenGLVertexBuffer::GetLayout() const
{
	return m_Layout;
}

void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_Layout = layout;
}

uint32_t OpenGLVertexBuffer::GetSize() const
{
	return m_Size;
}

RendererID OpenGLVertexBuffer::GetRendererID() const
{
	return m_RendererID;
}
}

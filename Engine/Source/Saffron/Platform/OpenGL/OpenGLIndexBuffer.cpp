#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t size) :
	OpenGLIndexBuffer(Buffer(static_cast<Uint8*>(data), size))
{
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Buffer& buffer) :
	m_Size(buffer.Size())
{
	m_LocalData = Buffer::Copy(buffer);

	Shared<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data(), GL_STATIC_DRAW);
	});
}

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size) :
	m_Size(size)
{
	// m_LocalData = Buffer(size);

	Shared<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, GL_DYNAMIC_DRAW);
	});
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteBuffers(1, &rendererID);
	});
}

void OpenGLIndexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	m_LocalData = Buffer::Copy(data, size);
	m_Size = size;
	Shared<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance, offset]()
	{
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
	});
}

void OpenGLIndexBuffer::SetData(const Buffer& buffer, Uint32 offset)
{
	SetData(buffer.Data(), buffer.Size(), offset);
}

void OpenGLIndexBuffer::Bind() const
{
	Shared<const OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
	});
}
}

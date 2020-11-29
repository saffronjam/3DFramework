#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{
static GLenum OpenGLUsage(VertexBuffer::Usage usage)
{
	switch ( usage )
	{
	case VertexBuffer::Usage::Static:	return GL_STATIC_DRAW;
	case VertexBuffer::Usage::Dynamic:	return GL_DYNAMIC_DRAW;
	case VertexBuffer::Usage::None:
	default:
		SE_CORE_ASSERT(false, "Unknown vertex buffer usage");
		return 0;
	}
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void *data, Uint32 size, Usage usage)
	: m_Size(size), m_Usage(usage)
{
	if ( data && m_Size )
	{
		m_LocalData = Buffer::Copy(data, m_Size);
	}
	Run::Later([this] { Create(); });
}

OpenGLVertexBuffer::OpenGLVertexBuffer(Uint32 size, Usage usage)
	: m_Size(size), m_Usage(usage)
{
	Run::Later([this] { Create(); });
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]() {
		glDeleteBuffers(1, &rendererID);
					 });
}

void OpenGLVertexBuffer::Bind() const
{
	auto instance = GetDynShared<OpenGLVertexBuffer>();
	Renderer::Submit([instance]() {
		glBindBuffer(GL_ARRAY_BUFFER, instance->m_RendererID);
					 });
}

void OpenGLVertexBuffer::SetData(void *buffer, Uint32 size, Uint32 offset)
{
	m_LocalData = Buffer::Copy(buffer, size);
	m_Size = size;
	auto instance = GetDynShared<OpenGLVertexBuffer>();
	Renderer::Submit([instance, offset]() {
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
					 });
}

void OpenGLVertexBuffer::SetData(const Buffer &buffer, Uint32 offset)
{
	m_LocalData = Buffer::Copy(buffer);
	m_Size = buffer.Size();
	auto instance = GetDynShared<OpenGLVertexBuffer>();
	Renderer::Submit([instance, offset]() {
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
					 });
}

void OpenGLVertexBuffer::Create()
{
	auto instance = GetDynShared<OpenGLVertexBuffer>();
	Renderer::Submit([instance]() mutable
					 {
						 glCreateBuffers(1, &instance->m_RendererID);
						 glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data(), OpenGLUsage(instance->m_Usage));
					 });
}
}

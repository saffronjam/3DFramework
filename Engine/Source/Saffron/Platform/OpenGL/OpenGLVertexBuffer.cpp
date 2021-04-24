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
	Debug::Assert(false, "Unknown vertex buffer usage");
	return 0;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, Uint32 size, VertexBufferUsage usage) :
	_size(size),
	_usage(usage)
{
	_localData = Buffer::Copy(data, size);
	Shared<OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->_rendererID);
		glNamedBufferData(instance->_rendererID, instance->_size, instance->_localData.Data(),
		                  OpenGLUsage(instance->_usage));
	});
}

OpenGLVertexBuffer::OpenGLVertexBuffer(Uint32 size, VertexBufferUsage usage) :
	_size(size),
	_usage(usage)
{
	Shared<OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->_rendererID);
		glNamedBufferData(instance->_rendererID, instance->_size, nullptr, OpenGLUsage(instance->_usage));
	});
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	GLuint rendererID = _rendererID;
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
		glBindBuffer(GL_ARRAY_BUFFER, instance->_rendererID);
	});
}

void OpenGLVertexBuffer::SetData(const void* data, Uint32 size, Uint32 offset)
{
	_localData = Buffer::Copy(data, size);
	_size = size;
	Shared<OpenGLVertexBuffer> instance = this;
	Renderer::Submit([instance, offset]()
	{
		glNamedBufferSubData(instance->_rendererID, offset, instance->_size, instance->_localData.Data());
	});
}

void OpenGLVertexBuffer::SetData(const Buffer& buffer, Uint32 offset)
{
	SetData(buffer.Data(), buffer.Size(), offset);
}

const VertexBufferLayout& OpenGLVertexBuffer::GetLayout() const
{
	return _layout;
}

void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	_layout = layout;
}

Uint32 OpenGLVertexBuffer::GetSize() const
{
	return _size;
}

RendererID OpenGLVertexBuffer::GetRendererID() const
{
	return _rendererID;
}
}

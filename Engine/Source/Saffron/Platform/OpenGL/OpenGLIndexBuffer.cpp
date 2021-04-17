#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, Uint32 size) :
	OpenGLIndexBuffer(Buffer(static_cast<Uint8*>(data), size))
{
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Buffer& buffer) :
	_size(buffer.Size())
{
	_localData = Buffer::Copy(buffer);

	Shared<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->_rendererID);
		glNamedBufferData(instance->_rendererID, instance->_size, instance->_localData.Data(), GL_STATIC_DRAW);
	});
}

OpenGLIndexBuffer::OpenGLIndexBuffer(Uint32 size) :
	_size(size)
{
	// _localData = Buffer(size);

	Shared<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glCreateBuffers(1, &instance->_rendererID);
		glNamedBufferData(instance->_rendererID, instance->_size, nullptr, GL_DYNAMIC_DRAW);
	});
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	GLuint rendererID = _rendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteBuffers(1, &rendererID);
	});
}

void OpenGLIndexBuffer::SetData(const void* data, Uint32 size, Uint32 offset)
{
	_localData = Buffer::Copy(data, size);
	_size = size;
	Shared<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance, offset]()
	{
		glNamedBufferSubData(instance->_rendererID, offset, instance->_size, instance->_localData.Data());
	});
}

void OpenGLIndexBuffer::SetData(const Buffer& buffer, Uint32 offset)
{
	SetData(buffer.Data(), buffer.Size(), offset);
}

Uint32 OpenGLIndexBuffer::GetCount() const
{
	return _size / sizeof(Uint32);
}

Uint32 OpenGLIndexBuffer::GetSize() const
{
	return _size;
}

RendererID OpenGLIndexBuffer::GetRendererID() const
{
	return _rendererID;
}

void OpenGLIndexBuffer::Bind() const
{
	Shared<const OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->_rendererID);
	});
}
}

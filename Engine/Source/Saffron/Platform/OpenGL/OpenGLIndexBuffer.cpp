#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{

OpenGLIndexBuffer::OpenGLIndexBuffer(Uint32 size)
	: m_RendererID(0), m_Size(size)
{
	Run::Later([this] { CreateDynamic(); });
}

OpenGLIndexBuffer::OpenGLIndexBuffer(void *data, Uint32 size)
	: m_Size(size), m_LocalData(Buffer::Copy(data, size))
{
	Run::Later([this] { CreateStatic(); });
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Buffer &buffer)
	: m_LocalData(Buffer::Copy(buffer))
{
	Run::Later([this] { CreateStatic(); });
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]() { glDeleteBuffers(1, &rendererID); });
}

void OpenGLIndexBuffer::Bind() const
{
	auto instance = GetDynShared<OpenGLIndexBuffer>();
	Renderer::Submit([instance]() {	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID); });
}

void OpenGLIndexBuffer::SetData(void *buffer, Uint32 size, Uint32 offset)
{
	m_LocalData = Buffer::Copy(buffer, size);
	m_Size = size;
	auto instance = GetDynShared<OpenGLIndexBuffer>();
	Renderer::Submit([instance, offset]() {
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
					 });
}

void OpenGLIndexBuffer::SetData(const Buffer &buffer, Uint32 offset)
{
	m_LocalData = Buffer::Copy(buffer);
	m_Size = buffer.Size();
	auto instance = GetDynShared<OpenGLIndexBuffer>();
	Renderer::Submit([instance, offset]() {
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
					 });
}

void OpenGLIndexBuffer::CreateDynamic()
{
	auto instance = GetDynShared<OpenGLIndexBuffer>();
	Renderer::Submit([instance]() mutable {
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, GL_DYNAMIC_DRAW);
					 });
}

void OpenGLIndexBuffer::CreateStatic()
{
	auto instance = GetDynShared<OpenGLIndexBuffer>();
	Renderer::Submit([instance]() mutable {
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data(), GL_STATIC_DRAW);
					 });
}
}

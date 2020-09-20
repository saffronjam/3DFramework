#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{

OpenGLIndexBuffer::OpenGLIndexBuffer(Uint32 size)
	: m_RendererID(0), m_Size(size)
{
	auto instance = Ref<OpenGLIndexBuffer>(this);
	Renderer::Submit([instance]() mutable {
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, GL_DYNAMIC_DRAW);
					 });
}

OpenGLIndexBuffer::OpenGLIndexBuffer(void *data, Uint32 size)
	: m_Size(size), m_LocalData(Buffer::Copy(data, size))
{
	auto instance = Ref<OpenGLIndexBuffer>(this);
	Renderer::Submit([instance]() mutable {
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data(), GL_STATIC_DRAW);
					 });
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Buffer &buffer)
	: m_LocalData(Buffer::Copy(buffer))
{
	auto instance = Ref<OpenGLIndexBuffer>(this);
	Renderer::Submit([instance]() mutable {
		glCreateBuffers(1, &instance->m_RendererID);
		glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data(), GL_STATIC_DRAW);
					 });
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]() { glDeleteBuffers(1, &rendererID); });
}

void OpenGLIndexBuffer::Bind() const
{
	Ref<const OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance]() {	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID); });
}

void OpenGLIndexBuffer::SetData(void *buffer, Uint32 size, Uint32 offset)
{
	m_LocalData = Buffer::Copy(buffer, size);
	m_Size = size;
	Ref<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance, offset]() {
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
					 });
}

void OpenGLIndexBuffer::SetData(const Buffer &buffer, Uint32 offset)
{
	m_LocalData = Buffer::Copy(buffer);
	m_Size = buffer.Size();
	Ref<OpenGLIndexBuffer> instance = this;
	Renderer::Submit([instance, offset]() {
		glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data());
					 });
}
}

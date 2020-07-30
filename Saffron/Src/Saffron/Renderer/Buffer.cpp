#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Buffer.h"
#include "Saffron/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Se {

BufferElement::BufferElement(ShaderDataType type, const std::string &name, bool normalized) :
	Name(name),
	Type(type),
	Size(ShaderDataTypeSize(type)),
	Offset(0),
	Normalized(normalized)
{
}

Uint32 BufferLayout::GetStride() const
{
	return m_Stride;
}

const std::vector<BufferElement> &BufferLayout::GetElements() const
{
	return m_Elements;
}

std::vector<BufferElement>::iterator BufferLayout::begin()
{
	return m_Elements.begin();
}

std::vector<BufferElement>::iterator BufferLayout::end()
{
	return m_Elements.end();
}

std::vector<BufferElement>::const_iterator BufferLayout::begin() const
{
	return m_Elements.begin();
}

std::vector<BufferElement>::const_iterator BufferLayout::end() const
{
	return m_Elements.end();
}

void BufferLayout::CalculateOffsetsAndStride()
{
	size_t offset = 0;
	m_Stride = 0;
	for ( auto &element : m_Elements )
	{
		element.Offset = offset;
		offset += element.Size;
		m_Stride += element.Size;
	}
}

Ref<VertexBuffer> VertexBuffer::Create(Uint32 size)
{
	switch ( Renderer::GetAPI() )
	{
	case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
	}

	SE_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(float *vertices, Uint32 size)
{
	switch ( Renderer::GetAPI() )
	{
	case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
	}

	SE_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(Uint32 *indices, Uint32 count)
{
	switch ( Renderer::GetAPI() )
	{
	case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, count);
	}

	SE_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

}

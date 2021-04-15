#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/VertexBuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Se
{
uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float: return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;
	case ShaderDataType::Mat3: return 4 * 3 * 3;
	case ShaderDataType::Mat4: return 4 * 4 * 4;
	case ShaderDataType::Int: return 4;
	case ShaderDataType::Int2: return 4 * 2;
	case ShaderDataType::Int3: return 4 * 3;
	case ShaderDataType::Int4: return 4 * 4;
	case ShaderDataType::Bool: return 1;
	}

	SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

VertexBufferElement::VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized):
	Name(name),
	Type(type),
	Size(ShaderDataTypeSize(type)),
	Offset(0),
	Normalized(normalized)
{
}

uint32_t VertexBufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case ShaderDataType::Float: return 1;
	case ShaderDataType::Float2: return 2;
	case ShaderDataType::Float3: return 3;
	case ShaderDataType::Float4: return 4;
	case ShaderDataType::Mat3: return 3 * 3;
	case ShaderDataType::Mat4: return 4 * 4;
	case ShaderDataType::Int: return 1;
	case ShaderDataType::Int2: return 2;
	case ShaderDataType::Int3: return 3;
	case ShaderDataType::Int4: return 4;
	case ShaderDataType::Bool: return 1;
	}

	SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

VertexBufferLayout::VertexBufferLayout()
{
}

VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements):
	m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

uint32_t VertexBufferLayout::GetStride() const
{
	return m_Stride;
}

const std::vector<VertexBufferElement>& VertexBufferLayout::GetElements() const
{
	return m_Elements;
}

std::vector<VertexBufferElement>::iterator VertexBufferLayout::begin()
{
	return m_Elements.begin();
}

std::vector<VertexBufferElement>::iterator VertexBufferLayout::end()
{
	return m_Elements.end();
}

std::vector<VertexBufferElement>::const_iterator VertexBufferLayout::begin() const
{
	return m_Elements.begin();
}

std::vector<VertexBufferElement>::const_iterator VertexBufferLayout::end() const
{
	return m_Elements.end();
}

void VertexBufferLayout::CalculateOffsetsAndStride()
{
	uint32_t offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_Stride += element.Size;
	}
}

Shared<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, VertexBufferUsage usage)
{
	switch (RendererAPI::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLVertexBuffer>::Create(data, size, usage);
	}
	SE_CORE_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}

Shared<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
{
	switch (RendererAPI::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLVertexBuffer>::Create(size, usage);
	}
	SE_CORE_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}
}

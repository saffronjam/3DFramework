#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/VertexBuffer.h"

namespace Se
{
Uint32 ShaderDataTypeSize(ShaderDataType type)
{
	switch ( type )
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
	default:
		SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}


///////////////////////////////////////////////////////////////
/// Vertex Buffer Element
///////////////////////////////////////////////////////////////

VertexBuffer::Element::Element(ShaderDataType type, std::string name, bool normalized) : Name(std::move(name)),
Type(type),
Size(ShaderDataTypeSize(
	type)),
	Normalized(normalized)
{
}

Uint32 VertexBuffer::Element::GetComponentCount() const
{
	switch ( Type )
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
	default:
		SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}


///////////////////////////////////////////////////////////////
/// Vertex Buffer Layout
///////////////////////////////////////////////////////////////

VertexBuffer::Layout::Layout(const std::initializer_list<Element> &elements) : m_Elements(elements)
{
	CalculateOffsetsAndStride();
}


///////////////////////////////////////////////////////////////
/// Vertex Buffer
///////////////////////////////////////////////////////////////

void VertexBuffer::Layout::CalculateOffsetsAndStride()
{
	Uint32 offset = 0;
	m_Stride = 0;
	for ( auto &element : m_Elements )
	{
		element.Offset = offset;
		offset += element.Size;
		m_Stride += element.Size;
	}
}
}

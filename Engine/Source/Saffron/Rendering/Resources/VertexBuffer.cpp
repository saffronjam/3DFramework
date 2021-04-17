#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/VertexBuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Se
{
Uint32 ShaderDataTypeSize(ShaderDataType type)
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

VertexBufferElement::VertexBufferElement(ShaderDataType type, const String& name, bool normalized):
	Name(name),
	Type(type),
	Size(ShaderDataTypeSize(type)),
	Offset(0),
	Normalized(normalized)
{
}

Uint32 VertexBufferElement::GetComponentCount() const
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

VertexBufferLayout::VertexBufferLayout(const InitializerList<VertexBufferElement>& elements):
	_elements(elements)
{
	CalculateOffsetsAndStride();
}

Uint32 VertexBufferLayout::GetStride() const
{
	return _stride;
}

const ArrayList<VertexBufferElement>& VertexBufferLayout::GetElements() const
{
	return _elements;
}

ArrayList<VertexBufferElement>::iterator VertexBufferLayout::begin()
{
	return _elements.begin();
}

ArrayList<VertexBufferElement>::iterator VertexBufferLayout::end()
{
	return _elements.end();
}

ArrayList<VertexBufferElement>::const_iterator VertexBufferLayout::begin() const
{
	return _elements.begin();
}

ArrayList<VertexBufferElement>::const_iterator VertexBufferLayout::end() const
{
	return _elements.end();
}

void VertexBufferLayout::CalculateOffsetsAndStride()
{
	Uint32 offset = 0;
	_stride = 0;
	for (auto& element : _elements)
	{
		element.Offset = offset;
		offset += element.Size;
		_stride += element.Size;
	}
}

Shared<VertexBuffer> VertexBuffer::Create(void* data, Uint32 size, VertexBufferUsage usage)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLVertexBuffer>::Create(data, size, usage);
	}
	SE_CORE_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}

Shared<VertexBuffer> VertexBuffer::Create(Uint32 size, VertexBufferUsage usage)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLVertexBuffer>::Create(size, usage);
	}
	SE_CORE_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}
}

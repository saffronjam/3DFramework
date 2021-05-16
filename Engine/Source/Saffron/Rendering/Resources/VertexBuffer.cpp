#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/VertexBuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Se
{
uint ShaderDataTypeSize(ShaderDataType type)
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

	Debug::Assert(false, "Unknown ShaderDataType!");;
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

uint VertexBufferElement::GetComponentCount() const
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

	Debug::Assert(false, "Unknown ShaderDataType!");;
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

uint VertexBufferLayout::GetStride() const
{
	return _stride;
}

const List<VertexBufferElement>& VertexBufferLayout::GetElements() const
{
	return _elements;
}

List<VertexBufferElement>::iterator VertexBufferLayout::begin()
{
	return _elements.begin();
}

List<VertexBufferElement>::iterator VertexBufferLayout::end()
{
	return _elements.end();
}

List<VertexBufferElement>::const_iterator VertexBufferLayout::begin() const
{
	return _elements.begin();
}

List<VertexBufferElement>::const_iterator VertexBufferLayout::end() const
{
	return _elements.end();
}

void VertexBufferLayout::CalculateOffsetsAndStride()
{
	uint offset = 0;
	_stride = 0;
	for (auto& element : _elements)
	{
		element.Offset = offset;
		offset += element.Size;
		_stride += element.Size;
	}
}

Shared<VertexBuffer> VertexBuffer::Create(void* data, uint size, VertexBufferUsage usage)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLVertexBuffer>::Create(data, size, usage);
	}
	Debug::Assert(false, "Unknown RendererAPI");;
	return nullptr;
}

Shared<VertexBuffer> VertexBuffer::Create(uint size, VertexBufferUsage usage)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLVertexBuffer>::Create(size, usage);
	}
	Debug::Assert(false, "Unknown RendererAPI");;
	return nullptr;
}
}

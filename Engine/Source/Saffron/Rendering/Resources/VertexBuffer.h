#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/RendererApi.h"


namespace Se
{
enum class ShaderDataType
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

static Uint32 ShaderDataTypeSize(ShaderDataType type);

struct VertexBufferElement
{
	String Name;
	ShaderDataType Type;
	Uint32 Size;
	Uint32 Offset;
	bool Normalized;

	VertexBufferElement() = default;
	VertexBufferElement(ShaderDataType type, const String& name, bool normalized = false);

	Uint32 GetComponentCount() const;
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();

	explicit VertexBufferLayout(const InitializerList<VertexBufferElement>& elements);

	Uint32 GetStride() const;
	const ArrayList<VertexBufferElement>& GetElements() const;

	ArrayList<VertexBufferElement>::iterator begin();
	ArrayList<VertexBufferElement>::iterator end();
	ArrayList<VertexBufferElement>::const_iterator begin() const;
	ArrayList<VertexBufferElement>::const_iterator end() const;

private:
	void CalculateOffsetsAndStride();

private:
	ArrayList<VertexBufferElement> m_Elements;
	Uint32 m_Stride = 0;
};

enum class VertexBufferUsage
{
	None = 0,
	Static = 1,
	Dynamic = 2
};

class VertexBuffer : public Managed
{
public:
	virtual ~VertexBuffer() = default;

	virtual void Bind() const = 0;

	virtual void SetData(const void* buffer, Uint32 size, Uint32 offset = 0) = 0;
	virtual void SetData(const Buffer& buffer, Uint32 offset = 0) = 0;

	virtual const VertexBufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const VertexBufferLayout& layout) = 0;

	virtual unsigned int GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	static Shared<VertexBuffer> Create(void* data, Uint32 size, VertexBufferUsage usage = VertexBufferUsage::Static);
	static Shared<VertexBuffer> Create(Uint32 size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
};
}

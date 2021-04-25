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

static uint ShaderDataTypeSize(ShaderDataType type);

struct VertexBufferElement
{
	String Name;
	ShaderDataType Type;
	uint Size;
	uint Offset;
	bool Normalized;

	VertexBufferElement() = default;
	VertexBufferElement(ShaderDataType type, const String& name, bool normalized = false);

	uint GetComponentCount() const;
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();
	VertexBufferLayout(const InitializerList<VertexBufferElement>& elements);

	uint GetStride() const;
	const ArrayList<VertexBufferElement>& GetElements() const;

	ArrayList<VertexBufferElement>::iterator begin();
	ArrayList<VertexBufferElement>::iterator end();
	ArrayList<VertexBufferElement>::const_iterator begin() const;
	ArrayList<VertexBufferElement>::const_iterator end() const;

private:
	void CalculateOffsetsAndStride();

private:
	ArrayList<VertexBufferElement> _elements;
	uint _stride = 0;
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

	virtual void SetData(const void* buffer, uint size, uint offset = 0) = 0;
	virtual void SetData(const Buffer& buffer, uint offset = 0) = 0;

	virtual const VertexBufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const VertexBufferLayout& layout) = 0;

	virtual unsigned int GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	static Shared<VertexBuffer> Create(void* data, uint size, VertexBufferUsage usage = VertexBufferUsage::Static);
	static Shared<VertexBuffer> Create(uint size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
};
}

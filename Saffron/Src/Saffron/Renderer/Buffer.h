#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"

namespace Se
{
enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static Uint32 ShaderDataTypeSize(ShaderDataType type);

struct BufferElement
{
	std::string Name;
	ShaderDataType Type;
	Uint32 Size;
	size_t Offset;
	bool Normalized;

	BufferElement() = default;
	BufferElement(ShaderDataType type, const std::string &name, bool normalized = false);

	Uint32 GetComponentCount() const;
};

class BufferLayout
{
public:
	BufferLayout() = default;

	BufferLayout(const std::initializer_list<BufferElement> &elements);

	Uint32 GetStride() const;
	const std::vector<BufferElement> &GetElements() const;

	std::vector<BufferElement>::iterator begin();
	std::vector<BufferElement>::iterator end();
	std::vector<BufferElement>::const_iterator begin() const;
	std::vector<BufferElement>::const_iterator end() const;

private:
	void CalculateOffsetsAndStride();

private:
	std::vector<BufferElement> m_Elements;
	Uint32 m_Stride = 0;
};

class VertexBuffer
{
public:
	virtual ~VertexBuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void SetData(const void *data, Uint32 size) = 0;

	virtual const BufferLayout &GetLayout() const = 0;
	virtual void SetLayout(const BufferLayout &layout) = 0;

	static Ref<VertexBuffer> Create(Uint32 size);
	static Ref<VertexBuffer> Create(float *vertices, Uint32 size);
};

// Saffron currently only supports 32-bit index buffers
class IndexBuffer
{
public:
	virtual ~IndexBuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual Uint32 GetCount() const = 0;

	static Ref<IndexBuffer> Create(Uint32 *indices, Uint32 count);
};
}

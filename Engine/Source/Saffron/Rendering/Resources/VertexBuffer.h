#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/RendererAPI.h"


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

static uint32_t ShaderDataTypeSize(ShaderDataType type);

struct VertexBufferElement
{
	std::string Name;
	ShaderDataType Type;
	uint32_t Size;
	uint32_t Offset;
	bool Normalized;

	VertexBufferElement() = default;
	VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

	uint32_t GetComponentCount() const;
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();

	explicit VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements);

	uint32_t GetStride() const;
	const std::vector<VertexBufferElement>& GetElements() const;

	std::vector<VertexBufferElement>::iterator begin();
	std::vector<VertexBufferElement>::iterator end();
	std::vector<VertexBufferElement>::const_iterator begin() const;
	std::vector<VertexBufferElement>::const_iterator end() const;

private:
	void CalculateOffsetsAndStride();

private:
	std::vector<VertexBufferElement> m_Elements;
	uint32_t m_Stride = 0;
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

	virtual void SetData(const void* buffer, uint32_t size, uint32_t offset = 0) = 0;
	virtual void SetData(const Buffer& buffer, uint32_t offset = 0) = 0;

	virtual const VertexBufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const VertexBufferLayout& layout) = 0;

	virtual unsigned int GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	static Shared<VertexBuffer> Create(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
	static Shared<VertexBuffer> Create(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
};
}

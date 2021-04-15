#pragma once

#include "Saffron/Rendering/Resources/VertexBuffer.h"

namespace Se
{
class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
	OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	virtual ~OpenGLVertexBuffer();

	void Bind() const override;

	void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	void SetData(const Buffer& buffer, uint32_t offset) override;

	const VertexBufferLayout& GetLayout() const override;

	void SetLayout(const VertexBufferLayout& layout) override;

	virtual uint32_t GetSize() const;

	virtual RendererID GetRendererID() const;
private:
	RendererID m_RendererID = 0;
	uint32_t m_Size;
	VertexBufferUsage m_Usage;
	VertexBufferLayout m_Layout;

	Buffer m_LocalData;
};

}

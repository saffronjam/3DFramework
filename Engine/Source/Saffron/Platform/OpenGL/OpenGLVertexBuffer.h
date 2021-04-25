#pragma once

#include "Saffron/Rendering/Resources/VertexBuffer.h"

namespace Se
{
class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer(void* data, uint size, VertexBufferUsage usage = VertexBufferUsage::Static);
	OpenGLVertexBuffer(uint size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	virtual ~OpenGLVertexBuffer();

	void Bind() const override;

	void SetData(const void* data, uint size, uint offset = 0) override;
	void SetData(const Buffer& buffer, uint offset) override;

	const VertexBufferLayout& GetLayout() const override;

	void SetLayout(const VertexBufferLayout& layout) override;

	uint GetSize() const override;

	RendererID GetRendererID() const override;
private:
	RendererID _rendererID = 0;
	uint _size;
	VertexBufferUsage _usage;
	VertexBufferLayout _layout;

	Buffer _localData;
};
}

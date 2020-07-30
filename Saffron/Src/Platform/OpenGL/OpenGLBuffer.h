#pragma once

#include "Saffron/Config.h"
#include "Saffron/Renderer/Buffer.h"

namespace Se
{
class OpenGLVertexBuffer : public VertexBuffer
{
public:
	explicit OpenGLVertexBuffer(Uint32 size);
	OpenGLVertexBuffer(float *vertices, Uint32 size);
	virtual ~OpenGLVertexBuffer();

	void Bind() const override;
	void Unbind() const override;

	void SetData(const void *data, Uint32 size) override;

	const BufferLayout &GetLayout() const override { return m_Layout; }
	void SetLayout(const BufferLayout &layout) override { m_Layout = layout; }
private:
	Uint32 m_RendererID;
	BufferLayout m_Layout;
};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
	OpenGLIndexBuffer(Uint32 *indices, Uint32 count);
	virtual ~OpenGLIndexBuffer();

	void Bind() const override;
	void Unbind() const override;

	Uint32 GetCount() const override;
private:
	Uint32 m_RendererID;
	Uint32 m_Count;
};
}

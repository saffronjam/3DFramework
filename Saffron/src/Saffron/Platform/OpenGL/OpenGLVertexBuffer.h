#pragma once

#include "Saffron/Renderer/VertexBuffer.h"

namespace Se
{
class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer(void *data, Uint32 size, Usage usage = Usage::Static);
	OpenGLVertexBuffer(Uint32 size, Usage usage = Usage::Dynamic);
	virtual ~OpenGLVertexBuffer();

	void Bind() const override;

	void SetData(void *buffer, Uint32 size, Uint32 offset = 0) override;
	void SetData(const Buffer &buffer, Uint32 offset = 0) override;

	Uint32 GetSize() const override { return m_Size; }
	RendererID GetRendererID() const override { return m_RendererID; }


private:
	RendererID m_RendererID = 0;
	Uint32 m_Size;
	Usage m_Usage;

	Buffer m_LocalData;
};

}

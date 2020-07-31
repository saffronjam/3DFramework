#pragma once

#include "Saffron/Renderer/VertexArray.h"

namespace Se
{
class OpenGLVertexArray : public VertexArray
{
public:
	OpenGLVertexArray();
	virtual ~OpenGLVertexArray();

	void Bind() const override;
	void Unbind() const override;

	void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) override;
	void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) override;

	const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const override { return m_VertexBuffers; }
	const Ref<IndexBuffer> &GetIndexBuffer() const override { return m_IndexBuffer; }
private:
	Uint32 m_RendererID;
	Uint32 m_VertexBufferIndex = 0;
	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;
};
}

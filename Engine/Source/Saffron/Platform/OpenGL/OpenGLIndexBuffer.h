#pragma once

#include "Saffron/Core/Buffer.h"
#include "Saffron/Renderer/IndexBuffer.h"

namespace Se
{
class OpenGLIndexBuffer : public IndexBuffer
{
public:
	explicit OpenGLIndexBuffer(Uint32 size);
	OpenGLIndexBuffer(void *data, Uint32 size);
	explicit OpenGLIndexBuffer(const Buffer &buffer);
	virtual ~OpenGLIndexBuffer();

	void Bind() const override;

	Uint32 GetCount() const override { return m_Size / sizeof(Uint32); }
	Uint32 GetSize() const override { return m_Size; }
	RendererID GetRendererID() const override { return m_RendererID; }

	void SetData(void *buffer, Uint32 size, Uint32 offset = 0) override;
	void SetData(const Buffer &buffer, Uint32 offset = 0) override;

private:
	void CreateDynamic();
	void CreateStatic();

private:
	Uint32 m_RendererID = 0;
	Uint32 m_Size{};

	Buffer m_LocalData;
};
}
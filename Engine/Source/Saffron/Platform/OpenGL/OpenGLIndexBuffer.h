#pragma once

#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/Resources/IndexBuffer.h"

namespace Se
{
class OpenGLIndexBuffer : public IndexBuffer
{
public:
	explicit OpenGLIndexBuffer(uint32_t size);
	OpenGLIndexBuffer(void* data, uint32_t size);
	explicit OpenGLIndexBuffer(const Buffer& buffer);
	virtual ~OpenGLIndexBuffer();

	void Bind() const override;

	void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	void SetData(const Buffer& buffer, Uint32 offset) override;

	virtual uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }

	virtual uint32_t GetSize() const { return m_Size; }

	virtual RendererID GetRendererID() const { return m_RendererID; }

private:
	RendererID m_RendererID = 0;
	uint32_t m_Size;

	Buffer m_LocalData;
};
}

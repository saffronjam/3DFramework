#pragma once

#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/Resources/IndexBuffer.h"

namespace Se
{
class OpenGLIndexBuffer : public IndexBuffer
{
public:
	explicit OpenGLIndexBuffer(uint size);
	OpenGLIndexBuffer(void* data, uint size);
	explicit OpenGLIndexBuffer(const Buffer& buffer);
	virtual ~OpenGLIndexBuffer();

	void Bind() const override;

	void SetData(const void* data, uint size, uint offset = 0) override;
	void SetData(const Buffer& buffer, uint offset) override;

	uint GetCount() const override;
	uint GetSize() const override;
	RendererID GetRendererID() const override;

private:
	RendererID _rendererID = 0;
	uint _size;

	Buffer _localData;
};
}

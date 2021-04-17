#pragma once

#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/Resources/IndexBuffer.h"

namespace Se
{
class OpenGLIndexBuffer : public IndexBuffer
{
public:
	explicit OpenGLIndexBuffer(Uint32 size);
	OpenGLIndexBuffer(void* data, Uint32 size);
	explicit OpenGLIndexBuffer(const Buffer& buffer);
	virtual ~OpenGLIndexBuffer();

	void Bind() const override;

	void SetData(const void* data, Uint32 size, Uint32 offset = 0) override;
	void SetData(const Buffer& buffer, Uint32 offset) override;

	Uint32 GetCount() const override;
	Uint32 GetSize() const override;
	RendererID GetRendererID() const override;

private:
	RendererID _rendererID = 0;
	Uint32 _size;

	Buffer _localData;
};
}

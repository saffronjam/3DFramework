#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Buffer.h"
#include "Saffron/Rendering/GraphicsResource.h"
#include "Saffron/Rendering/Enums.h"

namespace Se
{
class IndexBuffer : public GraphicsResource
{
public:
	explicit IndexBuffer(const Buffer& buffer, BufferFlags::Enum flags);
	~IndexBuffer();

	void Bind();

	static Shared<IndexBuffer> Create(const Buffer& buffer, BufferFlags::Enum flags = BufferFlags::None);

private:
	bgfx::IndexBufferHandle _handle;
};
}

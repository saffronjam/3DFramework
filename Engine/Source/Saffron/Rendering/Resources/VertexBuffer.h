#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Buffer.h"
#include "Saffron/Rendering/Enums.h"
#include "Saffron/Rendering/VertexLayout.h"

namespace Se
{
class VertexBuffer : public GraphicsResource
{
public:
	explicit VertexBuffer(const Buffer& buffer, const VertexLayout& layout, BufferFlags::Enum flags);
	~VertexBuffer();

	void Bind();

	static Shared<VertexBuffer> Create(const Buffer& buffer, const VertexLayout& layout, BufferFlags::Enum flags = BufferFlags::None);

private:
	bgfx::VertexBufferHandle _handle;
};
}

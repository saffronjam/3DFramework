#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/VertexBuffer.h"

namespace Se
{
VertexBuffer::VertexBuffer(const Buffer& buffer, const VertexLayout& layout, BufferFlags::Enum flags) :
	_handle(bgfx::createVertexBuffer(bgfx::makeRef(buffer.Data(), buffer.Size()), layout.GetNativeLayout(),
	                                 static_cast<Uint16>(flags)))
{
}

VertexBuffer::~VertexBuffer()
{
	bgfx::destroy(_handle);
}

void VertexBuffer::Bind()
{
	bgfx::setVertexBuffer(0, _handle);
}

Shared<VertexBuffer> VertexBuffer::Create(const Buffer& buffer, const VertexLayout& layout, BufferFlags::Enum flags)
{
	return CreateShared<VertexBuffer>(buffer, layout, flags);
}
}

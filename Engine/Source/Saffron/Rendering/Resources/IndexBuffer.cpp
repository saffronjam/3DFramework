#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/IndexBuffer.h"

namespace Se
{
IndexBuffer::IndexBuffer(const Buffer& buffer, BufferFlags::Enum flags) :
	_handle(bgfx::createIndexBuffer(bgfx::makeRef(buffer.Data(), buffer.Size()), static_cast<Uint16>(flags)))
{
}

IndexBuffer::~IndexBuffer()
{
	bgfx::destroy(_handle);
}

void IndexBuffer::Bind()
{
	bgfx::setIndexBuffer(_handle);
}

Shared<IndexBuffer> IndexBuffer::Create(const Buffer& buffer, BufferFlags::Enum flags)
{
	return CreateShared<IndexBuffer>(buffer, flags);
}
}

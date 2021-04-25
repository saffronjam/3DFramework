#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/Resource.h"
#include "Saffron/Rendering/RendererApi.h"

namespace Se
{
class IndexBuffer : public Resource
{
public:
	virtual ~IndexBuffer() = default;

	virtual void Bind() const = 0;

	virtual void SetData(const void* buffer, uint size, uint offset = 0) = 0;
	virtual void SetData(const Buffer& buffer, uint offset = 0) = 0;

	virtual uint GetCount() const = 0;
	virtual uint GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	static Shared<IndexBuffer> Create(uint size);
	static Shared<IndexBuffer> Create(void* data, uint size = 0);
	static Shared<IndexBuffer> Create(const Buffer& buffer);
};
}

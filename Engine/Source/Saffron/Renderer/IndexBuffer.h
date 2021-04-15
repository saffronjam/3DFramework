#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Resource/Resource.h"

namespace Se {

class IndexBuffer : public Resource
{
public:
	virtual ~IndexBuffer() = default;

	virtual void Bind() const = 0;

	virtual void SetData(void *buffer, Uint32 size, Uint32 offset = 0) = 0;
	virtual void SetData(const Buffer &buffer, Uint32 offset = 0) = 0;

	virtual Uint32 GetCount() const = 0;
	virtual Uint32 GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	static Shared<IndexBuffer> Create(Uint32 size);
	static Shared<IndexBuffer> Create(void *data, Uint32 size = 0);
	static Shared<IndexBuffer> Create(const Buffer &buffer);
};

}

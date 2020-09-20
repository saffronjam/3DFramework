#pragma once

#include "Saffron/Base.h"

namespace Se
{
typedef void(*RenderCommandFn)(void *);

class RenderCommandQueue
{
public:
	RenderCommandQueue();
	~RenderCommandQueue();

	void *Allocate(RenderCommandFn fn, Uint32 size);
	void Execute();

private:
	Uint8 *m_CommandBuffer;
	Uint8 *m_CommandBufferPtr;
	Uint32 m_CommandCount;

};
}


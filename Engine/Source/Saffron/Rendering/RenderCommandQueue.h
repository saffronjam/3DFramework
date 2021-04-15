#pragma once

#include "Saffron/Base.h"

namespace Se
{
typedef void(*RenderCommand)(void*);

class RenderCommandQueue
{
public:
	RenderCommandQueue();
	~RenderCommandQueue();

	void* Allocate(RenderCommand command, uint32_t size);

	void Execute();
private:
	uint8_t* m_CommandBuffer;
	uint8_t* m_CommandBufferPtr;
	uint32_t m_CommandCount = 0;
};
}


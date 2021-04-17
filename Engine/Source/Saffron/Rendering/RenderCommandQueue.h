#pragma once

#include "Saffron/Base.h"

namespace Se
{
typedef void (*RenderCommand)(void*);

class RenderCommandQueue
{
public:
	RenderCommandQueue();
	~RenderCommandQueue();

	void* Allocate(RenderCommand command, uint32_t size);

	void Execute();
private:
	uint8_t* _commandBuffer;
	uint8_t* _commandBufferPtr;
	uint32_t _commandCount = 0;
};
}

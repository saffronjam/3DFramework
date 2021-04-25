#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderCommandQueue.h"

namespace Se
{
constexpr auto CommandQueueSize = 10 * 1024;

RenderCommandQueue::RenderCommandQueue()
{
	_commandBuffer = new uint8_t[10 * 1024 * 1024]; // 10mb buffer
	_commandBufferPtr = _commandBuffer;
	memset(_commandBuffer, 0, 10 * 1024 * 1024);
}

RenderCommandQueue::~RenderCommandQueue()
{
	delete[] _commandBuffer;
}

void* RenderCommandQueue::Allocate(RenderCommand command, uint32_t size)
{
	// TODO: alignment
	*reinterpret_cast<RenderCommand*>(_commandBufferPtr) = command;
	_commandBufferPtr += sizeof(RenderCommand);

	*reinterpret_cast<uint32_t*>(_commandBufferPtr) = size;
	_commandBufferPtr += sizeof(uint32_t);

	void* memory = _commandBufferPtr;
	_commandBufferPtr += size;

	_commandCount++;
	return memory;
}

void RenderCommandQueue::Execute()
{
	//Log::CoreTrace("Executing {} commands", _commandBuffer.size());

	uchar* buffer = _commandBuffer;

	for (uint32_t i = 0; i < _commandCount; i++)
	{
		RenderCommand function = *(RenderCommand*)buffer;
		buffer += sizeof(RenderCommand);

		const uint32_t size = *reinterpret_cast<uint32_t*>(buffer);
		buffer += sizeof(uint32_t);
		function(buffer);
		buffer += size;
	}

	_commandBufferPtr = _commandBuffer;
	_commandCount = 0;
}
}

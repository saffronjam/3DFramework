#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderCommandQueue.h"

namespace Se
{
constexpr auto CommandQueueSize = 10 * 1024;

RenderCommandQueue::RenderCommandQueue()
{
	m_CommandBuffer = new uint8_t[10 * 1024 * 1024]; // 10mb buffer
	m_CommandBufferPtr = m_CommandBuffer;
	memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
}

RenderCommandQueue::~RenderCommandQueue()
{
	delete[] m_CommandBuffer;
}

void* RenderCommandQueue::Allocate(RenderCommand command, uint32_t size)
{
	// TODO: alignment
	*reinterpret_cast<RenderCommand*>(m_CommandBufferPtr) = command;
	m_CommandBufferPtr += sizeof(RenderCommand);

	*reinterpret_cast<uint32_t*>(m_CommandBufferPtr) = size;
	m_CommandBufferPtr += sizeof(uint32_t);

	void* memory = m_CommandBufferPtr;
	m_CommandBufferPtr += size;

	m_CommandCount++;
	return memory;
}

void RenderCommandQueue::Execute()
{
	//SE_CORE_TRACE("Executing {} commands", m_CommandBuffer.size());

	Uint8* buffer = m_CommandBuffer;

	for (uint32_t i = 0; i < m_CommandCount; i++)
	{
		RenderCommand function = *(RenderCommand*)buffer;
		buffer += sizeof(RenderCommand);

		const uint32_t size = *reinterpret_cast<uint32_t*>(buffer);
		buffer += sizeof(uint32_t);
		function(buffer);
		buffer += size;
	}

	m_CommandBufferPtr = m_CommandBuffer;
	m_CommandCount = 0;
}
}

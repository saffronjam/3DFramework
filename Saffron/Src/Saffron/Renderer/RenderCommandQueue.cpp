#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/RenderCommandQueue.h"

namespace Se
{

constexpr auto CommandQueueSize = 10 * 1024 * 1024;

RenderCommandQueue::RenderCommandQueue()
	: m_CommandBuffer(new Uint8[CommandQueueSize]),
	m_CommandBufferPtr(m_CommandBuffer)
{
	memset(m_CommandBuffer, 0, CommandQueueSize);
}

RenderCommandQueue::~RenderCommandQueue()
{
	delete[] m_CommandBuffer;
	m_CommandBuffer = nullptr;
	m_CommandBufferPtr = nullptr;
}

void *RenderCommandQueue::Allocate(RenderCommandFn fn, Uint32 size)
{
	*reinterpret_cast<RenderCommandFn *>(m_CommandBufferPtr) = fn;
	m_CommandBufferPtr += sizeof(RenderCommandFn);

	*reinterpret_cast<Uint32 *>(m_CommandBufferPtr) = size;
	m_CommandBufferPtr += sizeof(Uint32);

	void *memory = m_CommandBufferPtr;
	m_CommandBufferPtr += size;

	m_CommandCount++;
	return memory;
}

void RenderCommandQueue::Execute()
{
	SE_CORE_TRACE("RenderCommandQueue::Execute -- {0} commands, {1} bytes", m_CommandCount, (m_CommandBufferPtr - m_CommandBuffer));

	Uint8 *buffer = m_CommandBuffer;

	for ( Uint32 i = 0; i < m_CommandCount; i++ )
	{
		const RenderCommandFn function = *reinterpret_cast<RenderCommandFn *>(buffer);
		buffer += sizeof(RenderCommandFn);

		const Uint32 size = *reinterpret_cast<Uint32 *>(buffer);
		buffer += sizeof(Uint32);
		function(buffer);
		buffer += size;
	}

	m_CommandBufferPtr = m_CommandBuffer;
	m_CommandCount = 0;
}
}

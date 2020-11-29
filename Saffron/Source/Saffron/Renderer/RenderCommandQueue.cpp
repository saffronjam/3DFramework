#include "SaffronPCH.h"

#include "Saffron/Renderer/RenderCommandQueue.h"

namespace Se
{

constexpr auto CommandQueueSize = 10 * 1024;

RenderCommandQueue::RenderCommandQueue()
{
	m_CommandBuffer.reserve(CommandQueueSize);
}

void RenderCommandQueue::Submit(const RenderCommand &fn)
{
	m_CommandBuffer.push_back(fn);
	m_nCommands++;
}

void RenderCommandQueue::Execute()
{
	//SE_CORE_TRACE("Executing {} commands", m_CommandBuffer.size());

	for ( Uint32 i = 0; i < m_nCommands; i++ )
	{
		m_CommandBuffer[i]();
	}
	m_CommandBuffer.clear();
	m_nCommands = 0;
}
}

#pragma once

#include "Saffron/Base.h"

namespace Se
{
using RenderCommand = Function<void()>;

class RenderCommandQueue
{
public:
	RenderCommandQueue();

	void Submit(const RenderCommand &fn);
	void Execute();

private:
	ArrayList<RenderCommand> m_CommandBuffer;
	Uint32 m_nCommands = 0;
};
}


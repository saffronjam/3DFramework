#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/BufferClearPass.h"

namespace Se
{
BufferClearPass::BufferClearPass(const std::string& name) :
	RenderPass(name)
{
	RegisterInput("buffer", _buffer);
}

void BufferClearPass::Execute()
{
	_buffer->Clear();
}
}

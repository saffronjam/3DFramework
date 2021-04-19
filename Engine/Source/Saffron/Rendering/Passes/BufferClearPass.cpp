#include "SaffronPCH.h"

#include "Saffron/Rendering/Passes/BufferClearPass.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
BufferClearPass::BufferClearPass(String name) :
	RenderPass(Move(name))
{
	AddInput(CreateUnique<Stream::Input>("buffer", _buffer));
	AddOutput(CreateUnique<Stream::Output>("buffer", _buffer));
}

void BufferClearPass::Execute()
{
	Renderer::Begin(_buffer);
	Renderer::End();
}

void BufferClearPass::OnViewportResize(Uint32 width, Uint32 height)
{
}
}

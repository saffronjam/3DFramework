#include "SaffronPCH.h"

#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Platform/OpenGL/OpenGLRenderPass.h"

namespace Se
{
std::shared_ptr<RenderPass> RenderPass::Create(const Specification &specification)
{
	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:  return CreateShared<OpenGLRenderPass>(specification);
	default:
		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
}
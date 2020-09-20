#include "SaffronPCH.h"

#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Platform/OpenGL/OpenGLRenderPass.h"

namespace Se
{
Ref<RenderPass> RenderPass::Create(const Specification &specification)
{
	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:  return Ref<OpenGLRenderPass>::Create(specification);
	default:
		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
}
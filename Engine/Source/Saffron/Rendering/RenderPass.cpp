#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Platform/OpenGL/OpenGLRenderPass.h"

namespace Se
{
Shared<RenderPass> RenderPass::Create(const RenderPassSpecification& specification)
{
	switch (RendererAPI::Current())
	{
	case RendererApiType::None: SE_CORE_FALSE_ASSERT("RendererApi::None is currently not supported!");
		return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLRenderPass>::Create(specification);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI!");
	return nullptr;
}
}

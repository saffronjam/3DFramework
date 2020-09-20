#include "SaffronPCH.h"

#include "Saffron/Renderer/Pipeline.h"
#include "Saffron/Platform/OpenGL/OpenGLPipeline.h"

namespace Se
{
Ref<Pipeline> Pipeline::Create(const Specification &spec)
{
	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:    return nullptr;
	case RendererAPI::Type::OpenGL:  return Ref<OpenGLPipeline>::Create(spec);
	default:
		SE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
}

#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Pipeline.h"
#include "Saffron/Platform/OpenGL/OpenGLPipeline.h"

namespace Se
{
Shared<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
{
	switch (RendererAPI::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLPipeline>::Create(spec);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}
}

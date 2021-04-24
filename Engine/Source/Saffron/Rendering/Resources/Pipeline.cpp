#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Pipeline.h"
#include "Saffron/Platform/OpenGL/OpenGLPipeline.h"

namespace Se
{
Shared<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLPipeline>::Create(spec);
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}
}

#include "Saffron/SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Renderer/RendererAPI.h"

namespace Se
{

void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char *message,
	const void *userParam)
{
	switch ( severity )
	{
	case GL_DEBUG_SEVERITY_HIGH:			SE_CORE_CRITICAL("[OpenGL Debug HIGH] {0}", message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:			SE_CORE_ERROR("[OpenGL Debug MEDIUM] {0}", message); return;
	case GL_DEBUG_SEVERITY_LOW:				SE_CORE_WARN("[OpenGL Debug LOW] {0}", message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	SE_CORE_TRACE("[OpenGL Debug NOTIFICATION] {0}", message); return;
	default:								SE_CORE_ASSERT(false, "Unknown severity level!");
	}
}

void RendererAPI::Init()
{
	SE_PROFILE_FUNCTION();

#ifdef SE_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

	// TODO: Check if this is only a test
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Culling a depth testing
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glFrontFace(GL_CCW);

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Sampling and stencil test
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);

	// Setup renderer capabilities
	auto &capabilities = RendererAPI::GetCapabilities();
	capabilities.Vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
	capabilities.Renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
	capabilities.Version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	glGetIntegerv(GL_MAX_SAMPLES, &capabilities.MaxSamples);
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &capabilities.MaxAnisotropy);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &capabilities.MaxTextureUnits);

	for ( GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError() )
	{
		SE_CORE_ERROR("OpenGL Error: {0}", error);
	}

	LoadRequiredAssets();
}

void RendererAPI::Shutdown()
{
}

void RendererAPI::LoadRequiredAssets()
{
}

void RendererAPI::Clear(float r, float g, float b, float a)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RendererAPI::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}


void RendererAPI::DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest)
{
	if ( !depthTest )
		glDisable(GL_DEPTH_TEST);

	GLenum glPrimitiveType = 0;
	switch ( type )
	{
	case PrimitiveType::Triangles:
		glPrimitiveType = GL_TRIANGLES;
		break;
	case PrimitiveType::Lines:
		glPrimitiveType = GL_LINES;
		break;
	default:
		glPrimitiveType = GL_TRIANGLES;
		break;
	}

	glDrawElements(glPrimitiveType, count, GL_UNSIGNED_INT, nullptr);

	if ( !depthTest )
		glEnable(GL_DEPTH_TEST);
}

void RendererAPI::SetLineThickness(float thickness)
{
	glLineWidth(thickness);
}

}

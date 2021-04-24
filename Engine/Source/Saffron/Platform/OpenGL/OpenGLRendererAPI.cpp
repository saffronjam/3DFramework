#include "SaffronPCH.h"

#include <Glad/glad.h>

#include "Saffron/Rendering/RendererApi.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                             const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH: SE_CORE_ERROR("[OpenGL Debug HIGH] {0}", message);
		SE_CORE_ASSERT(false, "GL_DEBUG_SEVERITY_HIGH");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM: SE_CORE_WARN("[OpenGL Debug MEDIUM] {0}", message);
		break;
	case GL_DEBUG_SEVERITY_LOW: SE_CORE_INFO("[OpenGL Debug LOW] {0}", message);
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		// SE_CORE_TRACE("[OpenGL Debug NOTIFICATION] {0}", message);
		break;
	}
}

void RendererApi::Init()
{
	glDebugMessageCallback(OpenGLLogMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);

	auto& caps = GetCapabilities();

	caps.Vendor = (const char*)glGetString(GL_VENDOR);
	caps.Renderer = (const char*)glGetString(GL_RENDERER);
	caps.Version = (const char*)glGetString(GL_VERSION);

	glGetIntegerv(GL_MAX_SAMPLES, &caps.MaxSamples);
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.MaxAnisotropy);

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &caps.MaxTextureUnits);

	GLenum error = glGetError();
	while (error != GL_NO_ERROR)
	{
		SE_CORE_ERROR("OpenGL Error {0}", error);
		error = glGetError();
	}
}

void RendererApi::Shutdown()
{
}

void RendererApi::Clear(float r, float g, float b, float a)
{	
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RendererApi::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RendererApi::DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest)
{
	if (!depthTest)
		glDisable(GL_DEPTH_TEST);

	GLenum glPrimitiveType = 0;
	switch (type)
	{
	case PrimitiveType::Triangles: glPrimitiveType = GL_TRIANGLES;
		break;
	case PrimitiveType::Lines: glPrimitiveType = GL_LINES;
		break;
	}

	glDrawElements(glPrimitiveType, count, GL_UNSIGNED_INT, nullptr);

	if (!depthTest)
		glEnable(GL_DEPTH_TEST);
}

void RendererApi::SetLineThickness(float thickness)
{
	glLineWidth(thickness);
}
}

#include "Saffron/SaffronPCH.h"

#include <string>

#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Texture.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Se
{

Ref<Texture2D> Texture2D::Create(Uint32 width, Uint32 height)
{
	switch ( Renderer::GetAPI() )
	{
	case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}
}

Ref<Texture2D> Texture2D::Create(const std::string &path)
{
	switch ( Renderer::GetAPI() )
	{
	case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!");	return nullptr;
	}
}

}
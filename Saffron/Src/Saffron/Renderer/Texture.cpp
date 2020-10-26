#include "SaffronPCH.h"

#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Platform/OpenGL/OpenGLTexture.h"

namespace Se
{

Uint32 Texture::GetBPP(Format format)
{
	switch ( format )
	{
	case Format::RGB:    return 3;
	case Format::RGBA:   return 4;
	}
	return 0;
}

Uint32 Texture::CalculateMipMapCount(Uint32 width, Uint32 height)
{
	Uint32 levels = 1;
	while ( (width | height) >> levels )
		levels++;

	return levels;
}

Shared<Texture2D> Texture2D::Create(Format format, Uint32 width, Uint32 height, Wrap wrap)
{
	Shared<OpenGLTexture2D> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); result = nullptr;
	case RendererAPI::Type::OpenGL:	result = Shared<OpenGLTexture2D>::Create(format, width, height, wrap); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

Shared<Texture2D> Texture2D::Create(const Filepath &path, bool sRGB)
{
	Shared<OpenGLTexture2D> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); result = nullptr; break;
	case RendererAPI::Type::OpenGL:	result = Shared<OpenGLTexture2D>::Create(path, sRGB); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr; break;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

Shared<TextureCube> TextureCube::Create(Format format, Uint32 width, Uint32 height)
{
	Shared<OpenGLTextureCube> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); result = nullptr; break;
	case RendererAPI::Type::OpenGL: result = Shared<OpenGLTextureCube>::Create(format, width, height); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr; break;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

Shared<TextureCube> TextureCube::Create(const String &path)
{
	Shared<OpenGLTextureCube> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); result = nullptr; break;
	case RendererAPI::Type::OpenGL: result = Shared<OpenGLTextureCube>::Create(path); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); result = nullptr; break;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}
}

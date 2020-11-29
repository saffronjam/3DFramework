#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"
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

std::shared_ptr<Texture2D> Texture2D::Create(Format format, Uint32 width, Uint32 height, Wrap wrap)
{
	std::shared_ptr<Texture2D> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:
		SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		result = nullptr;
	case RendererAPI::Type::OpenGL:
		result = CreateShared<OpenGLTexture2D>(format, width, height, wrap);
		result->Initialize();
		break;
	default:
		SE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

std::shared_ptr<Texture2D> Texture2D::Create(const Filepath &path, bool sRGB)
{
	std::shared_ptr<Texture2D> result;

	//const size_t identifier = Misc::HashFilepath(path);
	//if ( ResourceManager::Exists(identifier) )
	//{
	//	return ResourceManager::Get(identifier)->GetStaticShared<Texture2D>();
	//}

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	result = CreateShared<OpenGLTexture2D>(path, sRGB); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
	}

	//if ( result )
	//{
	//	ResourceManager::Emplace(result);
	//}

	return result;
}

std::shared_ptr<TextureCube> TextureCube::Create(Format format, Uint32 width, Uint32 height)
{
	std::shared_ptr<OpenGLTextureCube> result;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL: result = CreateShared<OpenGLTextureCube>(format, width, height); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr; break;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}

std::shared_ptr<TextureCube> TextureCube::Create(const Filepath &path)
{
	std::shared_ptr<OpenGLTextureCube> result;

	const size_t identifier = Misc::HashFilepath(path);
	if ( ResourceManager::Exists(identifier) )
	{
		SE_CORE_INFO("Reusing texture CUBE: {}", path.string());
		return ResourceManager::Get(identifier)->GetDynShared<TextureCube>();
	}

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); result = nullptr; break;
	case RendererAPI::Type::OpenGL: result = CreateShared<OpenGLTextureCube>(path); break;
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); result = nullptr; break;
	}

	if ( result )
	{
		ResourceManager::Emplace(result);
	}

	return result;
}
}

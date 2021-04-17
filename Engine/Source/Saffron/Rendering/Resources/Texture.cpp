#include "SaffronPCH.h"

#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Rendering/Resources/Texture.h"
#include "Saffron/Platform/OpenGL/OpenGLTexture.h"

namespace Se
{
Uint32 Texture::GetBPP(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::RGB: return 3;
	case TextureFormat::RGBA: return 4;
	}
	return 0;
}

Uint32 Texture::CalculateMipMapCount(Uint32 width, Uint32 height)
{
	Uint32 levels = 1;
	while ((width | height) >> levels) levels++;

	return levels;
}

Shared<Texture2D> Texture2D::Create(TextureFormat format, Uint32 width, Uint32 height, TextureWrap wrap)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTexture2D>::Create(format, width, height, wrap);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}

Shared<Texture2D> Texture2D::Create(Filepath filepath, bool sRGB)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTexture2D>::Create(Move(filepath), sRGB);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}

Shared<TextureCube> TextureCube::Create(TextureFormat format, Uint32 width, Uint32 height)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTextureCube>::Create(format, width, height);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}

Shared<TextureCube> TextureCube::Create(Filepath filepath)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTextureCube>::Create(Move(filepath));
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}
}

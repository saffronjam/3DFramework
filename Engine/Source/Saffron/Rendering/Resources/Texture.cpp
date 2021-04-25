#include "SaffronPCH.h"

#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Rendering/Resources/Texture.h"
#include "Saffron/Platform/OpenGL/OpenGLTexture.h"

namespace Se
{
uint Texture::GetBPP(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::RGB: return 3;
	case TextureFormat::RGBA: return 4;
	}
	return 0;
}

uint Texture::CalculateMipMapCount(uint width, uint height)
{
	uint levels = 1;
	while ((width | height) >> levels) levels++;

	return levels;
}

Shared<Texture2D> Texture2D::Create(TextureFormat format, uint width, uint height, TextureWrap wrap)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTexture2D>::Create(format, width, height, wrap);
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}

Shared<Texture2D> Texture2D::Create(Filepath filepath, bool sRGB)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTexture2D>::Create(Move(filepath), sRGB);
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}

Shared<TextureCube> TextureCube::Create(TextureFormat format, uint width, uint height)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTextureCube>::Create(format, width, height);
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}

Shared<TextureCube> TextureCube::Create(Filepath filepath)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLTextureCube>::Create(Move(filepath));
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}
}

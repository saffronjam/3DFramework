#include "SaffronPCH.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Saffron/Platform/OpenGL/OpenGLTexture.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/RendererAPI.h"

namespace Se
{

static GLenum ConvertToOpenGLTextureFormat(Texture::Format format)
{
	switch ( format )
	{
	case Texture::Format::RGB:		return GL_RGB;
	case Texture::Format::RGBA:		return GL_RGBA;
	case Texture::Format::Float16:	return GL_RGBA16F;
	default:						SE_CORE_ASSERT(false, "Unknown texture format!"); return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////
/// Texture2D
//////////////////////////////////////////////////////////////////////////////////

OpenGLTexture2D::OpenGLTexture2D(Format format, Uint32 width, Uint32 height, Wrap wrap)
	: m_RendererID(0), m_Format(format), m_Wrap(wrap), m_Width(width), m_Height(height)
{
	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance]() mutable
					 {
						 glGenTextures(1, &instance->m_RendererID);
						 glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

						 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						 const GLenum wrap = instance->m_Wrap == Wrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
						 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
						 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
						 glTextureParameterf(instance->m_RendererID,
											 GL_TEXTURE_MAX_ANISOTROPY,
											 RendererAPI::GetCapabilities().MaxAnisotropy);
						 glTexImage2D(GL_TEXTURE_2D, 0,
									  ConvertToOpenGLTextureFormat(instance->m_Format),
									  instance->m_Width,
									  instance->m_Height, 0,
									  ConvertToOpenGLTextureFormat(instance->m_Format),
									  GL_UNSIGNED_BYTE, nullptr);
						 glBindTexture(GL_TEXTURE_2D, 0);
					 });

	m_ImageData.Allocate(width * height * GetBPP(m_Format));
}

OpenGLTexture2D::OpenGLTexture2D(const Filepath &filepath, bool sRGB)
	: m_Width(0), m_Height(0), m_Filepath(filepath)
{
	int width, height, channels;
	if ( stbi_is_hdr(m_Filepath.string().c_str()) )
	{
		SE_CORE_INFO("Loading HDR texture {0}, sRGB={1}", m_Filepath.string(), sRGB);

		m_ImageData = Buffer::Encapsulate(reinterpret_cast<Uint8 *>(stbi_loadf(m_Filepath.string().c_str(), &width, &height, &channels, 0)));
		SE_CORE_ASSERT(m_ImageData.Data(), "Could not read image!");
		m_IsHDR = true;
		m_Format = Format::Float16;
	}
	else
	{
		SE_CORE_INFO("Loading texture {0}, sRGB={1}", m_Filepath.string(), sRGB);
		m_ImageData = Buffer::Encapsulate(stbi_load(m_Filepath.string().c_str(), &width, &height, &channels, sRGB ? STBI_rgb : STBI_rgb_alpha));
		SE_CORE_ASSERT(m_ImageData.Data(), "Could not read image!");
		m_Format = Format::RGBA;
	}

	if ( !m_ImageData.Data() )
		return;

	m_Loaded = true;

	m_Width = width;
	m_Height = height;

	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance, sRGB]() mutable
					 {
						 // TODO: Consolidate properly
						 if ( sRGB )
						 {
							 glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
							 const int levels = Texture::CalculateMipMapCount(instance->m_Width, instance->m_Height);
							 glTextureStorage2D(instance->m_RendererID, levels, GL_SRGB8, instance->m_Width, instance->m_Height);
							 glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
							 glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

							 glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, GL_RGB, GL_UNSIGNED_BYTE, instance->m_ImageData.Data());
							 glGenerateTextureMipmap(instance->m_RendererID);
						 }
						 else
						 {
							 glGenTextures(1, &instance->m_RendererID);
							 glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

							 const GLenum internalFormat = ConvertToOpenGLTextureFormat(instance->m_Format);
							 const GLenum format = sRGB ? GL_SRGB8 : (instance->m_IsHDR ? GL_RGB : ConvertToOpenGLTextureFormat(instance->m_Format)); // HDR = GL_RGB for now
							 const GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
							 glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, instance->m_Width, instance->m_Height, 0, format, type, instance->m_ImageData.Data());
							 glGenerateMipmap(GL_TEXTURE_2D);

							 glBindTexture(GL_TEXTURE_2D, 0);
						 }
						 stbi_image_free(instance->m_ImageData.Data());
					 });
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]() { glDeleteTextures(1, &rendererID);	 });
}

void OpenGLTexture2D::Bind(Uint32 slot) const
{
	Shared<const OpenGLTexture2D> instance = this;
	Renderer::Submit([instance, slot]() { glBindTextureUnit(slot, instance->m_RendererID); });
}

Uint32 OpenGLTexture2D::GetMipLevelCount() const
{
	return CalculateMipMapCount(m_Width, m_Height);
}

void OpenGLTexture2D::Lock()
{
	m_Locked = true;
}

void OpenGLTexture2D::Unlock()
{
	m_Locked = false;
	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance]() { glTextureSubImage2D(
		instance->m_RendererID,
		0, 0, 0,
		instance->m_Width,
		instance->m_Height,
		ConvertToOpenGLTextureFormat(instance->m_Format),
		GL_UNSIGNED_BYTE,
		instance->m_ImageData.Data());
		});
}

void OpenGLTexture2D::Resize(Uint32 width, Uint32 height)
{
	SE_CORE_ASSERT(m_Locked, "Texture must be locked!");

	m_ImageData.Allocate(width * height * GetBPP(m_Format));
#if SE_DEBUG
	m_ImageData.ZeroInitialize();
#endif
}

Buffer &OpenGLTexture2D::GetWriteableBuffer()
{
	SE_CORE_ASSERT(m_Locked, "Texture must be locked!");
	return m_ImageData;
}

bool OpenGLTexture2D::operator==(const Texture &other) const
{
	return m_RendererID == dynamic_cast<const OpenGLTexture2D &>(other).m_RendererID;
}




//////////////////////////////////////////////////////////////////////////////////
/// TextureCube
//////////////////////////////////////////////////////////////////////////////////

OpenGLTextureCube::OpenGLTextureCube(Format format, Uint32 width, Uint32 height)
	: m_RendererID(0), m_Format(format), m_Width(width), m_Height(height)
{
	Uint32 levels = CalculateMipMapCount(width, height);
	Shared<OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, levels]() mutable
					 {
						 glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->m_RendererID);
						 glTextureStorage2D(instance->m_RendererID, levels, ConvertToOpenGLTextureFormat(instance->m_Format), instance->m_Width, instance->m_Height);
						 glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
						 glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
						 // glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, 16);
					 });
}

// TODO: Revisit this, as currently env maps are being loaded as equirectangular 2D images
//       so this is an old path
OpenGLTextureCube::OpenGLTextureCube(const Filepath &filepath)
	: m_Format(Format::None), m_Width(0), m_Height(0), m_Filepath(filepath)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(false);
	auto *stbiResult = stbi_load(m_Filepath.string().c_str(), &width, &height, &channels, STBI_rgb);
	m_ImageData = Buffer::Copy(stbiResult, width * height);
	stbi_image_free(stbiResult);

	m_Width = width;
	m_Height = height;
	m_Format = Format::RGB;

	Uint32 faceWidth = m_Width / 4;
	Uint32 faceHeight = m_Height / 3;
	SE_CORE_ASSERT(faceWidth == faceHeight, "Non-square faces!");

	std::array<Uint8 *, 6> faces{};
	for ( auto &face : faces )
		face = new Uint8[static_cast<Int32>(faceWidth * faceHeight) * 3]; // 3 BPP

	int faceIndex = 0;

	for ( size_t i = 0; i < 4; i++ )
	{
		for ( size_t y = 0; y < faceHeight; y++ )
		{
			const size_t yOffset = y + faceHeight;
			for ( size_t x = 0; x < faceWidth; x++ )
			{
				const size_t xOffset = x + i * faceWidth;
				faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(static_cast<int>(xOffset + yOffset) * m_Width) * 3 + 0];
				faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(static_cast<int>(xOffset + yOffset) * m_Width) * 3 + 1];
				faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(static_cast<int>(xOffset + yOffset) * m_Width) * 3 + 2];
			}
		}
		faceIndex++;
	}

	for ( size_t i = 0; i < 3; i++ )
	{
		// Skip the middle one
		if ( i == 1 )
			continue;

		for ( size_t y = 0; y < faceHeight; y++ )
		{
			const size_t yOffset = y + i * faceHeight;
			for ( size_t x = 0; x < faceWidth; x++ )
			{
				const size_t xOffset = x + faceWidth;
				faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(static_cast<int>(xOffset + yOffset) * m_Width) * 3 + 0];
				faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(static_cast<int>(xOffset + yOffset) * m_Width) * 3 + 1];
				faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(static_cast<int>(xOffset + yOffset) * m_Width) * 3 + 2];
			}
		}
		faceIndex++;
	}

	Shared<OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, faceWidth, faceHeight, faces]() mutable
					 {
						 glGenTextures(1, &instance->m_RendererID);
						 glBindTexture(GL_TEXTURE_CUBE_MAP, instance->m_RendererID);

						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
						 glTextureParameterf(instance->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

						 const auto format = ConvertToOpenGLTextureFormat(instance->m_Format);
						 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
						 glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);

						 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
						 glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);

						 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
						 glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

						 glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

						 glBindTexture(GL_TEXTURE_2D, 0);

						 for ( auto &face : faces )
							 delete[] face;
					 });
}

OpenGLTextureCube::~OpenGLTextureCube()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]() { glDeleteTextures(1, &rendererID); });
}

void OpenGLTextureCube::Bind(Uint32 slot) const
{
	Shared<const OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, slot]() { glBindTextureUnit(slot, instance->m_RendererID); });
}

Uint32 OpenGLTextureCube::GetMipLevelCount() const
{
	return CalculateMipMapCount(m_Width, m_Height);
}

bool OpenGLTextureCube::operator==(const Texture &other) const
{
	return m_RendererID == dynamic_cast<const OpenGLTextureCube &>(other).m_RendererID;
}
}

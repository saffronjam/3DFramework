#include "SaffronPCH.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Platform/OpenGL/OpenGLTexture.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RendererApi.h"

namespace Se
{
static GLenum HazelToOpenGLTextureFormat(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::RGB: return GL_RGB;
	case TextureFormat::RGBA: return GL_RGBA;
	case TextureFormat::Float16: return GL_RGBA16F;
	}
	SE_CORE_ASSERT(false, "Unknown texture format!");
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Texture2D
//////////////////////////////////////////////////////////////////////////////////

OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, Uint32 width, Uint32 height, TextureWrap wrap) :
	m_Format(format),
	m_Wrap(wrap),
	m_Width(width),
	m_Height(height)
{
	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glGenTextures(1, &instance->m_RendererID);
		glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLenum wrap = instance->m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTextureParameterf(instance->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY,
		                    RendererApi::GetCapabilities().MaxAnisotropy);

		glTexImage2D(GL_TEXTURE_2D, 0, HazelToOpenGLTextureFormat(instance->m_Format), instance->m_Width,
		             instance->m_Height, 0, HazelToOpenGLTextureFormat(instance->m_Format), GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	});

	m_ImageData.Allocate(width * height * GetBPP(m_Format));
}

OpenGLTexture2D::OpenGLTexture2D(Filepath filepath, bool srgb) :
	m_FilePath(TexturesLocation + Move(filepath).string())
{
	const auto filesize = FileIOManager::GetFileSize(m_FilePath);
	const auto filepathString = m_FilePath.string();
	int width, height, channels;
	if (stbi_is_hdr(filepathString.c_str()))
	{
		SE_CORE_INFO("Loading HDR texture {0}, srgb={1}", filepathString.c_str(), srgb);
		m_ImageData = Buffer(reinterpret_cast<Uint8*>(stbi_loadf(filepathString.c_str(), &width, &height, &channels, 0)), filesize);
		m_IsHDR = true;
		m_Format = TextureFormat::Float16;
	}
	else
	{
		SE_CORE_INFO("Loading texture {0}, srgb={1}", filepathString.c_str(), srgb);
		m_ImageData = Buffer(stbi_load(filepathString.c_str(), &width, &height, &channels,
		                               srgb ? STBI_rgb : STBI_rgb_alpha), filesize);
		SE_CORE_ASSERT(m_ImageData.Data(), "Could not read image!");
		m_Format = TextureFormat::RGBA;
	}

	if (!m_ImageData.Data()) return;

	m_Loaded = true;

	m_Width = width;
	m_Height = height;

	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance, srgb]() mutable
	{
		// TODO: Consolidate properly
		if (srgb)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
			int levels = CalculateMipMapCount(instance->m_Width, instance->m_Height);
			glTextureStorage2D(instance->m_RendererID, levels, GL_SRGB8, instance->m_Width, instance->m_Height);
			glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER,
			                    levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, GL_RGB,
			                    GL_UNSIGNED_BYTE, instance->m_ImageData.Data());
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

			GLenum internalFormat = HazelToOpenGLTextureFormat(instance->m_Format);
			GLenum format = srgb
				                ? GL_SRGB8
				                : (instance->m_IsHDR ? GL_RGB : HazelToOpenGLTextureFormat(instance->m_Format));
			// HDR = GL_RGB for now
			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, instance->m_Width, instance->m_Height, 0, format, type,
			             instance->m_ImageData.Data());
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		stbi_image_free(instance->m_ImageData.Data());
	});
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteTextures(1, &rendererID);
	});
}

bool OpenGLTexture2D::operator==(const Texture& other) const
{
	return m_RendererID == other.GetRendererID();
}

void OpenGLTexture2D::Bind(Uint32 slot) const
{
	Shared<const OpenGLTexture2D> instance = this;
	Renderer::Submit([instance, slot]()
	{
		glBindTextureUnit(slot, instance->m_RendererID);
	});
}

TextureFormat OpenGLTexture2D::GetFormat() const
{
	return m_Format;
}

Uint32 OpenGLTexture2D::GetWidth() const
{
	return m_Width;
}

Uint32 OpenGLTexture2D::GetHeight() const
{
	return m_Height;
}

void OpenGLTexture2D::Lock()
{
	m_Locked = true;
}

void OpenGLTexture2D::Unlock()
{
	m_Locked = false;
	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance]()
	{
		glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height,
		                    HazelToOpenGLTextureFormat(instance->m_Format), GL_UNSIGNED_BYTE,
		                    instance->m_ImageData.Data());
	});
}

void OpenGLTexture2D::Resize(Uint32 width, Uint32 height)
{
	SE_CORE_ASSERT(m_Locked, "Texture must be locked!");

	m_ImageData.Allocate(width * height * GetBPP(m_Format));
#if HZ_DEBUG
	m_ImageData.ZeroInitialize();
#endif
}

Buffer& OpenGLTexture2D::GetWriteableBuffer()
{
	SE_CORE_ASSERT(m_Locked, "Texture must be locked!");
	return m_ImageData;
}

const Filepath& OpenGLTexture2D::GetFilepath() const
{
	return m_FilePath;
}

RendererID OpenGLTexture2D::GetRendererID() const
{
	return m_RendererID;
}

bool OpenGLTexture2D::Loaded() const
{
	return m_Loaded;
}

Uint32 OpenGLTexture2D::GetMipLevelCount() const
{
	return CalculateMipMapCount(m_Width, m_Height);
}

//////////////////////////////////////////////////////////////////////////////////
// TextureCube
//////////////////////////////////////////////////////////////////////////////////

OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, Uint32 width, Uint32 height)
{
	m_Width = width;
	m_Height = height;
	m_Format = format;

	Uint32 levels = CalculateMipMapCount(width, height);
	Shared<OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, levels]() mutable
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->m_RendererID);
		glTextureStorage2D(instance->m_RendererID, levels, HazelToOpenGLTextureFormat(instance->m_Format),
		                   instance->m_Width, instance->m_Height);
		glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER,
		                    levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, 16);
	});
}

// TODO: Revisit this, as currently env maps are being loaded as equirectangular 2D images
//       so this is an old path
OpenGLTextureCube::OpenGLTextureCube(Filepath filepath) :
	m_FilePath(Move(filepath))
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(false);
	m_ImageData = stbi_load(m_FilePath.string().c_str(), &width, &height, &channels, STBI_rgb);

	m_Width = width;
	m_Height = height;
	m_Format = TextureFormat::RGB;

	Uint32 faceWidth = m_Width / 4;
	Uint32 faceHeight = m_Height / 3;
	SE_CORE_ASSERT(faceWidth == faceHeight, "Non-square faces!");

	std::array<uint8_t*, 6> faces;
	for (size_t i = 0; i < faces.size(); i++) faces[i] = new uint8_t[faceWidth * faceHeight * 3]; // 3 BPP

	int faceIndex = 0;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t y = 0; y < faceHeight; y++)
		{
			size_t yOffset = y + faceHeight;
			for (size_t x = 0; x < faceWidth; x++)
			{
				size_t xOffset = x + i * faceWidth;
				faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
				faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
				faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
			}
		}
		faceIndex++;
	}

	for (size_t i = 0; i < 3; i++)
	{
		// Skip the middle one
		if (i == 1) continue;

		for (size_t y = 0; y < faceHeight; y++)
		{
			size_t yOffset = y + i * faceHeight;
			for (size_t x = 0; x < faceWidth; x++)
			{
				size_t xOffset = x + faceWidth;
				faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
				faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
				faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
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
		glTextureParameterf(instance->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY,
		                    RendererApi::GetCapabilities().MaxAnisotropy);

		auto format = HazelToOpenGLTextureFormat(instance->m_Format);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE,
		             faces[2]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE,
		             faces[0]);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE,
		             faces[4]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE,
		             faces[5]);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE,
		             faces[1]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE,
		             faces[3]);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glBindTexture(GL_TEXTURE_2D, 0);

		for (size_t i = 0; i < faces.size(); i++) delete[] faces[i];

		stbi_image_free(instance->m_ImageData);
	});
}

OpenGLTextureCube::~OpenGLTextureCube()
{
	GLuint rendererID = m_RendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteTextures(1, &rendererID);
	});
}

bool OpenGLTextureCube::operator==(const Texture& other) const
{
	return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
}

TextureFormat OpenGLTextureCube::GetFormat() const
{
	return m_Format;
}

Uint32 OpenGLTextureCube::GetWidth() const
{
	return m_Width;
}

Uint32 OpenGLTextureCube::GetHeight() const
{
	return m_Height;
}

const Filepath& OpenGLTextureCube::GetFilepath() const
{
	return m_FilePath;
}

RendererID OpenGLTextureCube::GetRendererID() const
{
	return m_RendererID;
}

void OpenGLTextureCube::Bind(Uint32 slot) const
{
	Shared<const OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, slot]()
	{
		glBindTextureUnit(slot, instance->m_RendererID);
	});
}

Uint32 OpenGLTextureCube::GetMipLevelCount() const
{
	return CalculateMipMapCount(m_Width, m_Height);
}
}

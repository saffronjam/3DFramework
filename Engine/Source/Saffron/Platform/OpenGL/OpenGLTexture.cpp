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
	_format(format),
	_wrap(wrap),
	_width(width),
	_height(height)
{
	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance]() mutable
	{
		glGenTextures(1, &instance->_rendererID);
		glBindTexture(GL_TEXTURE_2D, instance->_rendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLenum wrap = instance->_wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTextureParameterf(instance->_rendererID, GL_TEXTURE_MAX_ANISOTROPY,
		                    RendererApi::GetCapabilities().MaxAnisotropy);

		glTexImage2D(GL_TEXTURE_2D, 0, HazelToOpenGLTextureFormat(instance->_format), instance->_width,
		             instance->_height, 0, HazelToOpenGLTextureFormat(instance->_format), GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	});

	_imageData.Allocate(width * height * GetBPP(_format));
}

OpenGLTexture2D::OpenGLTexture2D(Filepath filepath, bool srgb) :
	_filePath(TexturesLocation + Move(filepath).string())
{
	const auto filesize = FileIOManager::GetFileSize(_filePath);
	const auto filepathString = _filePath.string();
	int width, height, channels;
	if (stbi_is_hdr(filepathString.c_str()))
	{
		SE_CORE_INFO("Loading HDR texture {0}, srgb={1}", filepathString.c_str(), srgb);
		_imageData = Buffer(reinterpret_cast<Uint8*>(stbi_loadf(filepathString.c_str(), &width, &height, &channels, 0)), filesize);
		_isHDR = true;
		_format = TextureFormat::Float16;
	}
	else
	{
		SE_CORE_INFO("Loading texture {0}, srgb={1}", filepathString.c_str(), srgb);
		_imageData = Buffer(stbi_load(filepathString.c_str(), &width, &height, &channels,
		                               srgb ? STBI_rgb : STBI_rgb_alpha), filesize);
		SE_CORE_ASSERT(_imageData.Data(), "Could not read image!");
		_format = TextureFormat::RGBA;
	}

	if (!_imageData.Data()) return;

	_loaded = true;

	_width = width;
	_height = height;

	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance, srgb]() mutable
	{
		// TODO: Consolidate properly
		if (srgb)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &instance->_rendererID);
			int levels = CalculateMipMapCount(instance->_width, instance->_height);
			glTextureStorage2D(instance->_rendererID, levels, GL_SRGB8, instance->_width, instance->_height);
			glTextureParameteri(instance->_rendererID, GL_TEXTURE_MIN_FILTER,
			                    levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTextureParameteri(instance->_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureSubImage2D(instance->_rendererID, 0, 0, 0, instance->_width, instance->_height, GL_RGB,
			                    GL_UNSIGNED_BYTE, instance->_imageData.Data());
			glGenerateTextureMipmap(instance->_rendererID);
		}
		else
		{
			glGenTextures(1, &instance->_rendererID);
			glBindTexture(GL_TEXTURE_2D, instance->_rendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			GLenum internalFormat = HazelToOpenGLTextureFormat(instance->_format);
			GLenum format = srgb
				                ? GL_SRGB8
				                : (instance->_isHDR ? GL_RGB : HazelToOpenGLTextureFormat(instance->_format));
			// HDR = GL_RGB for now
			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, instance->_width, instance->_height, 0, format, type,
			             instance->_imageData.Data());
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		stbi_image_free(instance->_imageData.Data());
	});
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	GLuint rendererID = _rendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteTextures(1, &rendererID);
	});
}

bool OpenGLTexture2D::operator==(const Texture& other) const
{
	return _rendererID == other.GetRendererID();
}

void OpenGLTexture2D::Bind(Uint32 slot) const
{
	Shared<const OpenGLTexture2D> instance = this;
	Renderer::Submit([instance, slot]()
	{
		glBindTextureUnit(slot, instance->_rendererID);
	});
}

TextureFormat OpenGLTexture2D::GetFormat() const
{
	return _format;
}

Uint32 OpenGLTexture2D::GetWidth() const
{
	return _width;
}

Uint32 OpenGLTexture2D::GetHeight() const
{
	return _height;
}

void OpenGLTexture2D::Lock()
{
	_locked = true;
}

void OpenGLTexture2D::Unlock()
{
	_locked = false;
	Shared<OpenGLTexture2D> instance = this;
	Renderer::Submit([instance]()
	{
		glTextureSubImage2D(instance->_rendererID, 0, 0, 0, instance->_width, instance->_height,
		                    HazelToOpenGLTextureFormat(instance->_format), GL_UNSIGNED_BYTE,
		                    instance->_imageData.Data());
	});
}

void OpenGLTexture2D::Resize(Uint32 width, Uint32 height)
{
	SE_CORE_ASSERT(_locked, "Texture must be locked!");

	_imageData.Allocate(width * height * GetBPP(_format));
#if HZ_DEBUG
	_imageData.ZeroInitialize();
#endif
}

Buffer& OpenGLTexture2D::GetWriteableBuffer()
{
	SE_CORE_ASSERT(_locked, "Texture must be locked!");
	return _imageData;
}

const Filepath& OpenGLTexture2D::GetFilepath() const
{
	return _filePath;
}

RendererID OpenGLTexture2D::GetRendererID() const
{
	return _rendererID;
}

bool OpenGLTexture2D::Loaded() const
{
	return _loaded;
}

Uint32 OpenGLTexture2D::GetMipLevelCount() const
{
	return CalculateMipMapCount(_width, _height);
}

//////////////////////////////////////////////////////////////////////////////////
// TextureCube
//////////////////////////////////////////////////////////////////////////////////

OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, Uint32 width, Uint32 height)
{
	_width = width;
	_height = height;
	_format = format;

	Uint32 levels = CalculateMipMapCount(width, height);
	Shared<OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, levels]() mutable
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->_rendererID);
		glTextureStorage2D(instance->_rendererID, levels, HazelToOpenGLTextureFormat(instance->_format),
		                   instance->_width, instance->_height);
		glTextureParameteri(instance->_rendererID, GL_TEXTURE_MIN_FILTER,
		                    levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(instance->_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// glTextureParameterf(_rendererID, GL_TEXTURE_MAX_ANISOTROPY, 16);
	});
}

// TODO: Revisit this, as currently env maps are being loaded as equirectangular 2D images
//       so this is an old path
OpenGLTextureCube::OpenGLTextureCube(Filepath filepath) :
	_filePath(Move(filepath))
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(false);
	_imageData = stbi_load(_filePath.string().c_str(), &width, &height, &channels, STBI_rgb);

	_width = width;
	_height = height;
	_format = TextureFormat::RGB;

	Uint32 faceWidth = _width / 4;
	Uint32 faceHeight = _height / 3;
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
				faces[faceIndex][(x + y * faceWidth) * 3 + 0] = _imageData[(xOffset + yOffset * _width) * 3 + 0];
				faces[faceIndex][(x + y * faceWidth) * 3 + 1] = _imageData[(xOffset + yOffset * _width) * 3 + 1];
				faces[faceIndex][(x + y * faceWidth) * 3 + 2] = _imageData[(xOffset + yOffset * _width) * 3 + 2];
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
				faces[faceIndex][(x + y * faceWidth) * 3 + 0] = _imageData[(xOffset + yOffset * _width) * 3 + 0];
				faces[faceIndex][(x + y * faceWidth) * 3 + 1] = _imageData[(xOffset + yOffset * _width) * 3 + 1];
				faces[faceIndex][(x + y * faceWidth) * 3 + 2] = _imageData[(xOffset + yOffset * _width) * 3 + 2];
			}
		}
		faceIndex++;
	}

	Shared<OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, faceWidth, faceHeight, faces]() mutable
	{
		glGenTextures(1, &instance->_rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, instance->_rendererID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameterf(instance->_rendererID, GL_TEXTURE_MAX_ANISOTROPY,
		                    RendererApi::GetCapabilities().MaxAnisotropy);

		auto format = HazelToOpenGLTextureFormat(instance->_format);
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

		stbi_image_free(instance->_imageData);
	});
}

OpenGLTextureCube::~OpenGLTextureCube()
{
	GLuint rendererID = _rendererID;
	Renderer::Submit([rendererID]()
	{
		glDeleteTextures(1, &rendererID);
	});
}

bool OpenGLTextureCube::operator==(const Texture& other) const
{
	return _rendererID == ((OpenGLTextureCube&)other)._rendererID;
}

TextureFormat OpenGLTextureCube::GetFormat() const
{
	return _format;
}

Uint32 OpenGLTextureCube::GetWidth() const
{
	return _width;
}

Uint32 OpenGLTextureCube::GetHeight() const
{
	return _height;
}

const Filepath& OpenGLTextureCube::GetFilepath() const
{
	return _filePath;
}

RendererID OpenGLTextureCube::GetRendererID() const
{
	return _rendererID;
}

void OpenGLTextureCube::Bind(Uint32 slot) const
{
	Shared<const OpenGLTextureCube> instance = this;
	Renderer::Submit([instance, slot]()
	{
		glBindTextureUnit(slot, instance->_rendererID);
	});
}

Uint32 OpenGLTextureCube::GetMipLevelCount() const
{
	return CalculateMipMapCount(_width, _height);
}
}

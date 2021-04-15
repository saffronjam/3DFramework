#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap);
	OpenGLTexture2D(Filepath filepath, bool srgb);
	virtual ~OpenGLTexture2D();

	bool operator==(const Texture& other) const override;

	void Bind(uint32_t slot = 0) const override;

	TextureFormat GetFormat() const override;
	uint32_t GetWidth() const override;
	uint32_t GetHeight() const override;
	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	uint32_t GetMipLevelCount() const override;

	void Lock() override;
	void Unlock() override;

	void Resize(uint32_t width, uint32_t height) override;
	Buffer& GetWriteableBuffer() override;

	const Filepath& GetFilepath() const override;
	RendererID GetRendererID() const override;
	
	bool Loaded() const override;

private:
	RendererID m_RendererID;
	TextureFormat m_Format;
	TextureWrap m_Wrap = TextureWrap::Clamp;
	uint32_t m_Width, m_Height;

	Buffer m_ImageData;
	bool m_IsHDR = false;

	bool m_Locked = false;
	bool m_Loaded = false;

	Filepath m_FilePath;
};

class OpenGLTextureCube : public TextureCube
{
public:
	OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height);
	OpenGLTextureCube(Filepath filepath);
	virtual ~OpenGLTextureCube();

	bool operator==(const Texture& other) const override;

	void Bind(uint32_t slot = 0) const override;

	TextureFormat GetFormat() const override;

	uint32_t GetWidth() const override;

	uint32_t GetHeight() const override;

	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	uint32_t GetMipLevelCount() const override;

	const Filepath& GetFilepath() const override;

	RendererID GetRendererID() const override;


private:
	RendererID m_RendererID;
	TextureFormat m_Format;
	uint32_t m_Width, m_Height;

	unsigned char* m_ImageData;

	Filepath m_FilePath;
};
}

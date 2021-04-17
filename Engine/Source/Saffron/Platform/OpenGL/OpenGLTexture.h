#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(TextureFormat format, Uint32 width, Uint32 height, TextureWrap wrap);
	OpenGLTexture2D(Filepath filepath, bool srgb);
	virtual ~OpenGLTexture2D();

	bool operator==(const Texture& other) const override;

	void Bind(Uint32 slot = 0) const override;

	TextureFormat GetFormat() const override;
	Uint32 GetWidth() const override;
	Uint32 GetHeight() const override;
	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	Uint32 GetMipLevelCount() const override;

	void Lock() override;
	void Unlock() override;

	void Resize(Uint32 width, Uint32 height) override;
	Buffer& GetWriteableBuffer() override;

	const Filepath& GetFilepath() const override;
	RendererID GetRendererID() const override;

	bool Loaded() const override;

private:
	RendererID m_RendererID;
	TextureFormat m_Format;
	TextureWrap m_Wrap = TextureWrap::Clamp;
	Uint32 m_Width, m_Height;

	Buffer m_ImageData;
	bool m_IsHDR = false;

	bool m_Locked = false;
	bool m_Loaded = false;

	Filepath m_FilePath;
};

class OpenGLTextureCube : public TextureCube
{
public:
	OpenGLTextureCube(TextureFormat format, Uint32 width, Uint32 height);
	explicit OpenGLTextureCube(Filepath filepath);
	virtual ~OpenGLTextureCube();

	bool operator==(const Texture& other) const override;

	void Bind(Uint32 slot = 0) const override;

	TextureFormat GetFormat() const override;

	Uint32 GetWidth() const override;
	Uint32 GetHeight() const override;

	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	Uint32 GetMipLevelCount() const override;
	
	const Filepath& GetFilepath() const override;
	RendererID GetRendererID() const override;


private:
	RendererID m_RendererID;
	TextureFormat m_Format;
	Uint32 m_Width, m_Height;

	unsigned char* m_ImageData;

	Filepath m_FilePath;
};
}

#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(Format format, Uint32 width, Uint32 height, Wrap wrap);
	OpenGLTexture2D(Filepath filepath, bool sRGB);
	virtual ~OpenGLTexture2D();

	void Bind(Uint32 slot = 0) const override;

	size_t GetIdentifier() override;
	Format GetFormat() const override { return m_Format; }
	Uint32 GetWidth() const override { return m_Width; }
	Uint32 GetHeight() const override { return m_Height; }
	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	Uint32 GetMipLevelCount() const override;

	void Lock() override;
	void Unlock() override;

	void Resize(Uint32 width, Uint32 height) override;
	bool Loaded() const override { return m_Loaded; }

	Buffer &GetWriteableBuffer() override;
	const Filepath &GetFilepath() const override { return m_Filepath; }
	RendererID GetRendererID() const override { return m_RendererID; }

	bool operator==(const Texture &other) const override;

private:
	RendererID m_RendererID{};
	Format m_Format;
	Uint32 m_Width, m_Height;
	Wrap m_Wrap = Wrap::Clamp;

	Buffer m_ImageData;
	bool m_IsHDR = false;

	bool m_Locked = false;
	bool m_Loaded = false;

	Filepath m_Filepath;
};

class OpenGLTextureCube : public TextureCube
{
public:
	OpenGLTextureCube(Format format, Uint32 width, Uint32 height);
	OpenGLTextureCube(Filepath filepath);
	virtual ~OpenGLTextureCube();

	void Bind(Uint32 slot = 0) const override;

	// TODO: Implement proper texture identifiers
	size_t GetIdentifier() override { return UUID(); }
	Format GetFormat() const override { return m_Format; }
	Uint32 GetWidth() const override { return m_Width; }
	Uint32 GetHeight() const override { return m_Height; }
	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	Uint32 GetMipLevelCount() const override;

	const Filepath &GetFilepath() const override { return m_Filepath; }

	RendererID GetRendererID() const override { return m_RendererID; }

	bool operator==(const Texture &other) const override;

private:
	RendererID m_RendererID{};
	Format m_Format;
	Uint32 m_Width, m_Height;

	Buffer m_ImageData;

	Filepath m_Filepath;
};
}


#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(TextureFormat format, uint width, uint height, TextureWrap wrap);
	OpenGLTexture2D(Path filepath, bool srgb);
	virtual ~OpenGLTexture2D();

	bool operator==(const Texture& other) const override;

	void Bind(uint slot = 0) const override;

	TextureFormat GetFormat() const override;
	uint GetWidth() const override;
	uint GetHeight() const override;
	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	uint GetMipLevelCount() const override;

	void Lock() override;
	void Unlock() override;

	void Resize(uint width, uint height) override;
	Buffer& GetWriteableBuffer() override;

	const Path& GetFilepath() const override;
	RendererID GetRendererID() const override;

	bool Loaded() const override;

private:
	RendererID _rendererID;
	TextureFormat _format;
	TextureWrap _wrap = TextureWrap::Clamp;
	uint _width, _height;

	Buffer _imageData;
	bool _isHDR = false;

	bool _locked = false;
	bool _loaded = false;

	Path _filePath;
};

class OpenGLTextureCube : public TextureCube
{
public:
	OpenGLTextureCube(TextureFormat format, uint width, uint height);
	explicit OpenGLTextureCube(Path filepath);
	virtual ~OpenGLTextureCube();

	bool operator==(const Texture& other) const override;

	void Bind(uint slot = 0) const override;

	TextureFormat GetFormat() const override;

	uint GetWidth() const override;
	uint GetHeight() const override;

	// This function currently returns the expected number of mips based on image size,
	// not present mips in data
	uint GetMipLevelCount() const override;
	
	const Path& GetFilepath() const override;
	RendererID GetRendererID() const override;


private:
	RendererID _rendererID;
	TextureFormat _format;
	uint _width, _height;

	unsigned char* _imageData;

	Path _filePath;
};
}

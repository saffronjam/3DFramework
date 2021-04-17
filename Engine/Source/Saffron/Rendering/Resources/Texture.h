#pragma once


#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Rendering/Resource.h"
#include "Saffron/Rendering/RendererApi.h"

namespace Se
{
enum class TextureFormat
{
	None = 0,
	RGB = 1,
	RGBA = 2,
	Float16 = 3
};

enum class TextureWrap
{
	None = 0,
	Clamp = 1,
	Repeat = 2
};

class Texture : public Resource
{
public:
	virtual ~Texture() = default;

	virtual void Bind(Uint32 slot = 0) const = 0;

	virtual TextureFormat GetFormat() const = 0;

	virtual Uint32 GetWidth() const = 0;
	virtual Uint32 GetHeight() const = 0;
	virtual Uint32 GetMipLevelCount() const = 0;

	virtual RendererID GetRendererID() const = 0;

	static Uint32 GetBPP(TextureFormat format);
	static Uint32 CalculateMipMapCount(Uint32 width, Uint32 height);

	virtual bool operator==(const Texture& other) const = 0;

protected:
	static constexpr const char* TexturesLocation = "Assets/Textures/";
};

class Texture2D : public Texture
{
public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;

	virtual void Resize(Uint32 width, Uint32 height) = 0;
	virtual Buffer& GetWriteableBuffer() = 0;

	virtual bool Loaded() const = 0;

	virtual const Filepath& GetFilepath() const = 0;

	static Shared<Texture2D> Create(TextureFormat format, Uint32 width, Uint32 height,
	                                TextureWrap wrap = TextureWrap::Clamp);
	static Shared<Texture2D> Create(Filepath filepath, bool sRGB = false);
};

class TextureCube : public Texture
{
public:
	virtual const Filepath& GetFilepath() const = 0;

	static Shared<TextureCube> Create(TextureFormat format, Uint32 width, Uint32 height);
	static Shared<TextureCube> Create(Filepath filepath);
};
}

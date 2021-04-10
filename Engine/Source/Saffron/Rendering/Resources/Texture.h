#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Enums.h"
#include "Saffron/Rendering/GraphicsResource.h"
#include "Saffron/Rendering/Resources/Sampler.h"

namespace Se
{
class DepthStencil;

struct TextureInfo
{
	TextureFormat Format;
	int StorageSize;
	Uint16 Width;
	Uint16 Height;
	Uint16 Depth;
	Uint16 Layers;
	Uint8 MipCount;
	Uint8 BitsPerPixel;
	bool IsCubeMap;
};

class Texture : public GraphicsResource
{
public:
	explicit Texture(Filepath filepath, Uint32 slot = 0, TextureFlags::Enum flags = TextureFlags::None);
	~Texture();
	
	void Bind(const Sampler::Ptr& sampler, TextureFlags::Enum flags = TextureFlags::None);

	Uint32 GetSlot() const;
	const TextureInfo& GetInfo() const;
	bgfx::TextureHandle GetNativeHandle() const;

	static Shared<Texture> Create(Filepath filepath, Uint32 slot = 0, TextureFlags::Enum flags = TextureFlags::None);


private:
	bgfx::TextureHandle _handle;
	Uint32 _slot;
	Filepath _filepath;
	TextureInfo _info;

	static constexpr const char* TextureFolder = "../Assets/Textures/";
};
}

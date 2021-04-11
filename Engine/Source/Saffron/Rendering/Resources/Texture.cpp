#include "SaffronPCH.h"

#include <bimg/bimg.h>

#include "Saffron/Common/FileIOManager.h"
#include "Saffron/Rendering/Resources/Texture.h"


namespace Se
{
Texture::Texture(Filepath filepath, Uint32 slot, TextureFlags::Enum flags) :
	_slot(slot),
	_filepath(Move(filepath))
{
	OutputStringStream oss;
	const size_t fileSize = FileIOManager::Read(_filepath, oss);
	const String data = oss.str();

	if (fileSize > 0)
	{
		bimg::ImageContainer imageContainer;
		const bool result = bimg::imageParse(imageContainer, data.c_str(), static_cast<Uint32>(fileSize));

		if (result)
		{
			const Buffer buffer(static_cast<Uint8*>(imageContainer.m_data), imageContainer.m_size);

			if (imageContainer.m_cubeMap)
			{
				_handle = bgfx::createTextureCube(static_cast<uint16_t>(imageContainer.m_width),
				                                  1 < imageContainer.m_numMips, imageContainer.m_numLayers,
				                                  static_cast<bgfx::TextureFormat::Enum>(imageContainer.m_format),
				                                  static_cast<Uint64>(flags),
				                                  reinterpret_cast<const bgfx::Memory*>(buffer.Data()));
			}
			else if (1 < imageContainer.m_depth)
			{
				_handle = bgfx::createTexture3D(static_cast<uint16_t>(imageContainer.m_width),
				                                static_cast<uint16_t>(imageContainer.m_height),
				                                static_cast<uint16_t>(imageContainer.m_depth),
				                                1 < imageContainer.m_numMips,
				                                static_cast<bgfx::TextureFormat::Enum>(imageContainer.m_format),
				                                static_cast<Uint64>(flags),
				                                reinterpret_cast<const bgfx::Memory*>(buffer.Data()));
			}
			else if (bgfx::isTextureValid(0, false, imageContainer.m_numLayers,
			                              static_cast<bgfx::TextureFormat::Enum>(imageContainer.m_format),
			                              static_cast<Uint64>(flags)))
			{
				_handle = bgfx::createTexture2D(static_cast<uint16_t>(imageContainer.m_width),
				                                static_cast<uint16_t>(imageContainer.m_height),
				                                1 < imageContainer.m_numMips, imageContainer.m_numLayers,
				                                static_cast<bgfx::TextureFormat::Enum>(imageContainer.m_format),
				                                static_cast<Uint64>(flags),
				                                reinterpret_cast<const bgfx::Memory*>(buffer.Data()));
			}

			if (bgfx::isValid(_handle))
			{
				bgfx::setName(_handle, reinterpret_cast<const char*>(filepath.c_str()));
			}

			bgfx::TextureInfo bgfxTextureInfo;
			bgfx::calcTextureSize(bgfxTextureInfo, static_cast<uint16_t>(imageContainer.m_width),
			                      static_cast<uint16_t>(imageContainer.m_height),
			                      static_cast<uint16_t>(imageContainer.m_depth), imageContainer.m_cubeMap,
			                      1 < imageContainer.m_numMips, imageContainer.m_numLayers,
			                      static_cast<bgfx::TextureFormat::Enum>(imageContainer.m_format));

			_info.StorageSize = bgfxTextureInfo.storageSize;
			_info.Width = bgfxTextureInfo.width;
			_info.Height = bgfxTextureInfo.height;
			_info.Depth = bgfxTextureInfo.depth;
			_info.Layers = bgfxTextureInfo.numLayers;
			_info.MipCount = bgfxTextureInfo.numMips;
			_info.BitsPerPixel = bgfxTextureInfo.bitsPerPixel;
			_info.IsCubeMap = bgfxTextureInfo.cubeMap;
		}
	}
}

Texture::~Texture()
{
	bgfx::destroy(_handle);
}

void Texture::Bind(const Sampler::Ptr& sampler, TextureFlags::Enum flags)
{
	bgfx::setTexture(_slot, sampler->GetNativeHandle(), _handle, static_cast<Uint32>(flags));
}

Uint32 Texture::GetSlot() const
{
	return _slot;
}

const TextureInfo& Texture::GetInfo() const
{
	return _info;
}

bgfx::TextureHandle Texture::GetNativeHandle() const
{
	return _handle;
}

Shared<Texture> Texture::Create(Filepath filepath, Uint32 slot, TextureFlags::Enum flags)
{
	return CreateShared<Texture>(Move(filepath), slot, flags);
}
}

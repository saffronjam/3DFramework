#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"

namespace Se
{
enum class ImageFormat
{
	None = 0,
	RGB,
	RGBA,
	RGBA16f,
	RGBA32f,
	RG16f,
	RG32f,
	Depth32f,
	Depth24Stencil8,

	// Defaults
	Depth = Depth24Stencil8,
};

using ImageUsage = uint;

enum ImageUsageFlags : uint
{
	ImageUsage_DepthStencil = 0,
	ImageUsage_RenderTarget = 1 << 0,
	ImageUsage_ShaderResource = 1 << 1
};

struct ImageSpec
{
	uint Width, Height;
	ImageUsage Usage;
	ImageFormat Format;
	uint* InitialData = nullptr;
	uint InitialDataSize = 0;
};

class Image
{
public:
	explicit Image(const ImageSpec& spec);

	template <typename T>
	auto RenderViewAs() -> T&;
	template <typename T>
	auto RenderViewAs() const -> const T&;
	auto ShaderView() -> ID3D11ShaderResourceView&;
	auto ShaderView() const -> const ID3D11ShaderResourceView&;

	static auto Create(const ImageSpec& spec) -> std::shared_ptr<Image>;

private:
	ComPtr<ID3D11Texture2D> _nativeTexture;

	ComPtr<ID3D11View> _nativeRenderView;
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView;

	ImageSpec _spec;
};

template <typename T>
auto Image::RenderViewAs() -> T&
{
	return *PtrAs<T>(_nativeRenderView).Get();
}

template <typename T>
auto Image::RenderViewAs() const -> const T&
{
	return *PtrAs<T>(_nativeRenderView).Get();
}

namespace Utils
{
inline auto ToDxgiTextureFormat(ImageFormat format) -> DXGI_FORMAT
{
	switch (format)
	{
	case ImageFormat::RGBA: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat::None: return static_cast<DXGI_FORMAT>(0);
	default: break;
	}
	Debug::Break("Image format not supported");
	return static_cast<DXGI_FORMAT>(0);
}

inline auto ToD3D11BindFlag(ImageUsage usage) -> uint
{
	switch (usage)
	{
	case ImageUsage_DepthStencil: return D3D11_BIND_DEPTH_STENCIL;
	case ImageUsage_RenderTarget: return D3D11_BIND_RENDER_TARGET;
	case ImageUsage_ShaderResource: return D3D11_BIND_SHADER_RESOURCE;
	default: break;
	}
	Debug::Break("Image usage not supported");
	return 0;
}
}
}

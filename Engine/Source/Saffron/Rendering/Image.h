#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"

namespace Se
{
enum class ImageFormat
{
	None = 0,
	R,
	RGBA,
	RGBA16,
	RGBA16f,
	RGBA32f,
	RG16f,
	RG32f,

	SRGB,

	Depth32f,
	Depth24Stencil8,

	// Defaults
	Depth = Depth24Stencil8,
};

using ImageUsage = uint;

enum ImageUsageFlags : uint
{
	ImageUsage_DepthStencil = 1 << 0,
	ImageUsage_RenderTarget = 1 << 1,
	ImageUsage_ShaderResource = 1 << 2
};

struct ImageSpec
{
	uint Width, Height;
	ImageUsage Usage;
	ImageFormat Format;
};

class Image
{
public:
	explicit Image(const ImageSpec& spec, const uint* initialData = nullptr);

	template <typename T>
	auto RenderViewAs() -> T&;
	template <typename T>
	auto RenderViewAs() const -> const T&;
	auto ShaderView() -> ID3D11ShaderResourceView&;
	auto ShaderView() const -> const ID3D11ShaderResourceView&;

	auto Width() const -> uint;
	auto Height() const -> uint;
	auto Format() const -> ImageFormat;
	auto Usage() const->ImageUsage;

	static auto Create(const ImageSpec& spec) -> std::shared_ptr<Image>;
	static auto CreateFromBackBuffer() -> std::shared_ptr<Image>;

private:
	Image() = default;

private:
	ComPtr<ID3D11Texture2D> _nativeTexture;
	ComPtr<ID3D11View> _nativeRenderView;
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView;

	ImageSpec _spec{};

	friend class Texture;
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
	case ImageFormat::RGBA16f: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case ImageFormat::RGBA32f: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ImageFormat::RG16f: return DXGI_FORMAT_R16G16_FLOAT;
	case ImageFormat::Depth24Stencil8: return DXGI_FORMAT_R24G8_TYPELESS;
	case ImageFormat::Depth32f: return DXGI_FORMAT_R32_TYPELESS;
	default: break;
	}
	throw SaffronException("Image format not supported");
}

inline auto ToDxgiDepthStencilFormat(ImageFormat format) -> DXGI_FORMAT
{
	switch (format)
	{
	case ImageFormat::Depth24Stencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case ImageFormat::Depth32f: return DXGI_FORMAT_D32_FLOAT;
	default: break;
	}
	throw SaffronException("Image format not supported");
}

inline auto ToDxgiRenderTargetFormat(ImageFormat format) -> DXGI_FORMAT
{
	switch (format)
	{
	case ImageFormat::RGBA: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat::RGBA16f: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case ImageFormat::RGBA32f: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ImageFormat::RG16f: return DXGI_FORMAT_R16G16_FLOAT;
	default: break;
	}
	throw SaffronException("Image format not supported");
}

inline auto ToDxgiShaderResourceFormat(ImageFormat format) -> DXGI_FORMAT
{
	switch (format)
	{
	case ImageFormat::R: return DXGI_FORMAT_R8_UNORM;
	case ImageFormat::RGBA: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat::RGBA16: return DXGI_FORMAT_R16G16B16A16_UNORM;
	case ImageFormat::RGBA16f: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case ImageFormat::RGBA32f: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ImageFormat::RG16f: return DXGI_FORMAT_R16G16_FLOAT;
	case ImageFormat::Depth24Stencil8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case ImageFormat::Depth32f: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	default: break;
	}
	throw SaffronException("Image format not supported");
}


inline auto ToSaffronFormat(DXGI_FORMAT format) -> ImageFormat
{
	switch (format)
	{
	case DXGI_FORMAT_R8_UNORM: return ImageFormat::R;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return ImageFormat::RGBA;
	case DXGI_FORMAT_R16G16B16A16_UNORM: return ImageFormat::RGBA16;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return ImageFormat::RGBA16f;
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return ImageFormat::RGBA32f;
	case DXGI_FORMAT_R16G16_FLOAT: return ImageFormat::RG16f;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return ImageFormat::SRGB;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return ImageFormat::Depth24Stencil8;
	default: break;
	}
	throw SaffronException("Dxgi format not supported");
}

inline auto ToD3D11Format(ImageFormat format) -> DXGI_FORMAT
{
	switch (format)
	{
	case ImageFormat::R: return DXGI_FORMAT_R8_UNORM;
	case ImageFormat::RGBA: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat::RGBA16: return DXGI_FORMAT_R16G16B16A16_UNORM;
	case ImageFormat::RGBA16f: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case ImageFormat::RGBA32f: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ImageFormat::RG16f: return DXGI_FORMAT_R16G16_FLOAT;
	case ImageFormat::SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case ImageFormat::Depth24Stencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
	default: break;
	}
	throw SaffronException("Saffron format not supported");
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
	throw SaffronException("Image usage not supported");
}
}
}

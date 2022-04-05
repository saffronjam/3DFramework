#pragma once

#include "Saffron/Base.h"

namespace Se
{
enum class ImageFormat
{
	None = 0,
	R,
	R32f,
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
	int ArraySize = 1;
};

class Image
{
public:
	explicit Image(const ImageSpec& spec, const uint* initialData = nullptr);

	auto Width() const -> uint;
	auto Height() const -> uint;
	auto Format() const -> ImageFormat;
	auto Usage() const->ImageUsage;

	static auto Create(const ImageSpec& spec) -> std::shared_ptr<Image>;
	static auto CreateFromBackBuffer() -> std::shared_ptr<Image>;

private:
	Image() = default;

private:

	ImageSpec _spec{};
};
}

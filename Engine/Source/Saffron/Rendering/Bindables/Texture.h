#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Image.h"
#include "Saffron/Rendering/Bindables/Sampler.h"

namespace DirectX
{
struct TexMetadata;
class ScratchImage;
}

namespace Se
{
struct TextureSpec
{
	SamplerWrap SamplerWrap = SamplerWrap::Wrap;
	SamplerFilter SamplerFilter = SamplerFilter::Bilinear;
	bool GenerateMips = true;
	bool SRGB = false;
	bool CreateSampler = true;
	TextureUsage Usage = TextureUsage_ShaderResource;
};

struct RendererPackage;


class Texture : public Bindable
{
public:
	explicit Texture(const TextureSpec& spec, uint slot);
	Texture(uint width, uint height, ImageFormat format, const uchar* data, const TextureSpec& spec, uint slot);
	Texture(const std::filesystem::path& path, const TextureSpec& spec, uint slot);
	Texture(const std::shared_ptr<Image>& image, const TextureSpec& spec, uint slot);

	void Bind() const override;
	void Unbind() const override;

	auto Width() const -> uint;
	auto Height() const -> uint;
	auto Format() const -> ImageFormat;
	auto Loaded() const -> bool;
	auto BindFlags() const -> ShaderBindFlags;

	void SetImage(const std::shared_ptr<Image>& image);
	void SetBindFlags(ShaderBindFlags flags);

	auto NativeHandle() -> ID3D11Texture2D&;
	auto NativeHandle() const -> const ID3D11Texture2D&;
	auto ShaderView() -> ID3D11ShaderResourceView&;
	auto ShaderView() const -> const ID3D11ShaderResourceView&;
	auto UnorderedView() -> ID3D11UnorderedAccessView&;
	auto UnorderedView() const -> const ID3D11UnorderedAccessView&;

	static auto Create(const TextureSpec& spec = TextureSpec(), uint slot = 0) -> std::shared_ptr<Texture>;;
	static auto Create(
		uint width,
		uint height,
		ImageFormat format = ImageFormat::RGBA,
		const uchar* data = nullptr,
		const TextureSpec& spec = TextureSpec(),
		uint slot = 0
	) -> std::shared_ptr<Texture>;
	static auto Create(
		const std::filesystem::path& path,
		const TextureSpec& spec = TextureSpec(),
		uint slot = 0
	) -> std::shared_ptr<Texture>;
	static auto Create(
		const std::shared_ptr<Image>& image,
		const TextureSpec& spec = TextureSpec(),
		uint slot = 0
	) -> std::shared_ptr<Texture>;

private:
	static auto CreateSrv(
		const RendererPackage& package,
		ID3D11Texture2D& tex,
		const DirectX::TexMetadata& meta
	) -> ComPtr<ID3D11ShaderResourceView>;

	static auto CreateUav(
		const RendererPackage& package,
		ID3D11Texture2D& tex,
		const DirectX::TexMetadata& meta
	) -> ComPtr<ID3D11UnorderedAccessView>;

	static auto LoadWicFromFile(
		const RendererPackage& package,
		const std::filesystem::path& path,
		const TextureSpec& spec
	) -> std::tuple<ComPtr<ID3D11Texture2D>, DirectX::TexMetadata>;

	static auto LoadHdrFromFile(
		const RendererPackage& package,
		const std::filesystem::path& path,
		const TextureSpec& spec
	) -> std::tuple<ComPtr<ID3D11Texture2D>, DirectX::TexMetadata>;

	static auto CreateFromScratchAndMeta(
		const RendererPackage& package,
		const DirectX::ScratchImage& scratch,
		const DirectX::TexMetadata& meta,
		const TextureSpec& spec
	) -> ComPtr<ID3D11Texture2D>;

private:
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView{};
	ComPtr<ID3D11UnorderedAccessView> _nativeUnorderedAccessView{};
	ComPtr<ID3D11Texture2D> _nativeTexture{};
	std::shared_ptr<Sampler> _sampler;

	TextureSpec _spec;
	ImageFormat _format = ImageFormat::None;
	ShaderBindFlags _bindFlags = BindFlag_PS;
	uint _width = 0, _height = 0;
	uint _slot = 0;
	bool _loaded = false;
	std::optional<std::filesystem::path> _path;

	std::vector<uchar> _dataBuffer;
};


class TextureCube : public Bindable
{
public:
	TextureCube(uint width, uint height, ImageFormat format, const TextureSpec& spec, uint slot);

	void Bind() const override;
	void Unbind() const override;

	auto NativeHandle() -> ID3D11Texture2D&;
	auto NativeHandle() const -> const ID3D11Texture2D&;
	auto ShaderView() -> ID3D11ShaderResourceView&;
	auto ShaderView() const -> const ID3D11ShaderResourceView&;
	auto UnorderedView() -> ID3D11UnorderedAccessView&;
	auto UnorderedView() const -> const ID3D11UnorderedAccessView&;

	auto Width() const -> uint;
	auto Height() const -> uint;
	auto Format() const -> ImageFormat;
	auto BindFlags() const -> ShaderBindFlags;
	auto Usage() const -> TextureUsage;

	void SetBindFlags(ShaderBindFlags flags);
	void SetUsage(TextureUsage usage);

	static auto Create(
		uint width,
		uint height,
		ImageFormat format,
		const TextureSpec& spec = TextureSpec(),
		uint slot = 0
	) -> std::shared_ptr<TextureCube>;

private:
	static auto CreateSrv(
		const RendererPackage& package,
		ID3D11Texture2D& tex,
		const DirectX::TexMetadata& meta
	) -> ComPtr<ID3D11ShaderResourceView>;

	static auto CreateUav(
		const RendererPackage& package,
		ID3D11Texture2D& tex,
		const DirectX::TexMetadata& meta
	) -> ComPtr<ID3D11UnorderedAccessView>;

private:
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView{};
	ComPtr<ID3D11UnorderedAccessView> _nativeUnorderedAccessView{};
	ComPtr<ID3D11Texture2D> _nativeTexture{};
	std::shared_ptr<Sampler> _sampler;

	uint _width = 0;
	uint _height = 0;
	ImageFormat _format = ImageFormat::None;
	ShaderBindFlags _bindFlags = BindFlag_PS;
	TextureSpec _spec;

	uint _slot = 0;
};

namespace Utils
{
static auto ImageFormatToMemorySize(ImageFormat format) -> size_t;

inline auto ToD3D11TextureBindFlag(TextureUsage usage) -> uint
{
	switch (usage)
	{
	case TextureUsage_UnorderedAccess: return D3D11_BIND_UNORDERED_ACCESS;
	case TextureUsage_ShaderResource: return D3D11_BIND_SHADER_RESOURCE;
	default: break;
	}
	throw SaffronException("Image usage not supported");
}

inline auto ToD3D11TextureBindFlags(TextureUsage usages) -> uint
{
	uint result = 0;

	if (usages & TextureUsage_UnorderedAccess)
	{
		result |= ToD3D11TextureBindFlag(TextureUsage_UnorderedAccess);
	}

	if (usages & TextureUsage_ShaderResource)
	{
		result |= ToD3D11TextureBindFlag(TextureUsage_ShaderResource);
	}

	return result;
}
}
}

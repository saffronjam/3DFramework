#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Image.h"
#include "Saffron/Rendering/Bindables/Sampler.h"

namespace Se
{
struct TextureSpec
{
	SamplerWrap SamplerWrap = SamplerWrap::Mirror;
	SamplerFilter SamplerFilter = SamplerFilter::Bilinear;
	bool GenerateMips = true;
	bool SRGB = false;
	bool CreateSampler = true;
};

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
	auto Loaded() const -> bool;

	void SetImage(const std::shared_ptr<Image>& image);

	auto NativeHandle() -> ID3D11Texture2D&;
	auto NativeHandle() const -> const ID3D11Texture2D&;
	auto ShaderView() -> ID3D11ShaderResourceView&;
	auto ShaderView() const -> const ID3D11ShaderResourceView&;

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
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView;
	ComPtr<ID3D11Texture2D> _nativeTexture;
	std::shared_ptr<Sampler> _sampler;

	TextureSpec _spec;
	ImageFormat _format = ImageFormat::None;
	uint _width = 0, _height = 0;
	uint _slot = 0;
	bool _loaded = false;
	std::optional<std::filesystem::path> _path;

	std::vector<uchar> _dataBuffer;
};

namespace Utils
{
static size_t ImageFormatToMemorySize(ImageFormat format);
}
}

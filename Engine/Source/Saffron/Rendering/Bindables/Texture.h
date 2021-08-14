#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Image.h"

namespace Se
{
class Texture : public Bindable
{
public:
	explicit Texture(uint slot);
	Texture(uint width, uint height, ImageFormat format, uint slot);
	Texture(const std::filesystem::path& path, uint slot);
	Texture(const std::shared_ptr<Image>& image, uint slot);

	void Bind() const override;
	void Unbind() const override;

	auto Width() const -> uint;
	auto Height() const -> uint;

	void SetImage(const std::shared_ptr<Image>& image);

	auto NativeHandle() -> ID3D11Texture2D&;
	auto NativeHandle() const -> const ID3D11Texture2D&;
	auto ShaderView() -> ID3D11ShaderResourceView&;
	auto ShaderView() const -> const ID3D11ShaderResourceView&;

	static auto Create(uint slot = 0) -> std::shared_ptr<Texture>;;
	static auto Create(
		uint width,
		uint height,
		ImageFormat format = ImageFormat::RGBA,
		uint slot = 0
	) -> std::shared_ptr<Texture>;
	static auto Create(const std::filesystem::path& path, uint slot = 0) -> std::shared_ptr<Texture>;
	static auto Create(const std::shared_ptr<Image>& image, uint slot = 0) -> std::shared_ptr<Texture>;

private:
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView;
	ComPtr<ID3D11Texture2D> _nativeTexture;

	ImageFormat _format = ImageFormat::None;
	uint _width = 0, _height = 0;
	uint _slot = 0;
	std::optional<std::filesystem::path> _path;
};
}

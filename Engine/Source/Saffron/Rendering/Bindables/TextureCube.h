#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Image.h"

namespace Se
{
class TextureCube : public Bindable
{
public:
	TextureCube(uint width, uint height, ImageFormat format, uint slot);

	void Bind() const override;

	auto ShaderView() const -> const ID3D11ShaderResourceView&;

	auto Width() const -> uint;
	auto Height() const -> uint;

	static auto Create(uint width, uint height, ImageFormat format, uint slot = 0) -> std::shared_ptr<TextureCube>;

private:
	ComPtr<ID3D11Texture2D> _nativeTexture2d;
	ComPtr<ID3D11ShaderResourceView> _nativeShaderResourceView;

	uint _width = 0;
	uint _height = 0;
	ImageFormat _format = ImageFormat::None;
	uint _slot = 0;
};
}

#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/TextureCube.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
TextureCube::TextureCube(uint width, uint height, ImageFormat format, uint slot) :
	_width(width),
	_height(height),
	_format(format),
	_slot(slot)
{
	SetInitializer(
		[=]
		{
			const auto inst = ShareThisAs<TextureCube>();
			Renderer::Submit(
				[=](const RendererPackage& package)
				{
					D3D11_TEXTURE2D_DESC td = {};
					td.Width = width;
					td.Height = height;
					td.MipLevels = 1;
					td.ArraySize = 6;
					td.Format = Utils::ToDxgiTextureFormat(format);
					td.SampleDesc.Count = 1;
					td.SampleDesc.Quality = 0;
					td.Usage = D3D11_USAGE_DEFAULT;
					td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					td.CPUAccessFlags = 0;
					td.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


					Color colors[100 * 100 ];
					for(auto& color : colors)
					{
						color = Color{ 1.0f, 1.0f, 1.0f, 1.0f };
					}


					std::array<D3D11_SUBRESOURCE_DATA, 6> subResData;
					for (int i = 0; i < 6; i++)
					{
						subResData[i].pSysMem = colors;
						subResData[i].SysMemPitch = 100 * 4;
						subResData[i].SysMemSlicePitch = 0;
					}

					auto hr = package.Device.CreateTexture2D(&td, subResData.data(), &inst->_nativeTexture2d);
					ThrowIfBad(hr);

					D3D11_SHADER_RESOURCE_VIEW_DESC sd = {};
					sd.Format = td.Format;
					sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					sd.TextureCube.MostDetailedMip = 0;
					sd.TextureCube.MipLevels = 1;

					hr = package.Device.CreateShaderResourceView(
						inst->_nativeTexture2d.Get(),
						&sd,
						&inst->_nativeShaderResourceView
					);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void TextureCube::Bind() const
{
	const auto inst = ShareThisAs<const TextureCube>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.PSSetShaderResources(inst->_slot, 1, inst->_nativeShaderResourceView.GetAddressOf());
		}
	);
}

auto TextureCube::NativeHandle() -> ID3D11Texture2D&
{
	return *_nativeTexture2d.Get();
}

auto TextureCube::NativeHandle() const -> const ID3D11Texture2D&
{
	return *_nativeTexture2d.Get();
}

auto TextureCube::ShaderView() const -> const ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto TextureCube::Width() const -> uint
{
	return _width;
}

auto TextureCube::Height() const -> uint
{
	return _height;
}

auto TextureCube::Create(uint width, uint height, ImageFormat format, uint slot) -> std::shared_ptr<TextureCube>
{
	return BindableStore::Add<TextureCube>(width, height, format, slot);
}
}

#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Image.h"

namespace Se
{
Image::Image(const ImageSpec& spec) :
	_spec(spec)
{
	auto& device = Renderer::Device();

	auto bindFlags = 0u;

	const bool depthStencil = spec.Usage & ImageUsage_DepthStencil;
	const bool renderTarget = spec.Usage & ImageUsage_RenderTarget;
	const bool shaderResource = spec.Usage & ImageUsage_ShaderResource;

	Debug::Assert(!(depthStencil && renderTarget), "Image can't be both depth stencil and render target");

	if (depthStencil)
	{
		bindFlags |= Utils::ToD3D11BindFlag(ImageUsage_DepthStencil);
	}
	if (renderTarget)
	{
		bindFlags |= Utils::ToD3D11BindFlag(ImageUsage_RenderTarget);
	}
	if (shaderResource)
	{
		bindFlags |= Utils::ToD3D11BindFlag(ImageUsage_ShaderResource);
	}

	D3D11_TEXTURE2D_DESC td = {};
	td.Format = Utils::ToDxgiTextureFormat(_spec.Format);
	td.Width = _spec.Width;
	td.Height = _spec.Height;
	td.ArraySize = 1;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = bindFlags;
	td.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	if (spec.InitialData != nullptr)
	{
		sd.pSysMem = spec.InitialData;
	}

	auto hr = device.CreateTexture2D(&td, spec.InitialData ? &sd : nullptr, &_nativeTexture);
	ThrowIfBad(hr);

	if (depthStencil)
	{
		ThrowIfBad(hr);
	}
	else if (renderTarget)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rd = {};
		rd.Format = td.Format;
		rd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rd.Texture2D.MipSlice = 0;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		hr = device.CreateRenderTargetView(_nativeTexture.Get(), &rd, &renderTargetView);
		ThrowIfBad(hr);

		_nativeRenderView = renderTargetView;
	}

	if (shaderResource || true)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = td.MipLevels;
		srvd.Texture2D.MostDetailedMip = 0;

		hr = device.CreateShaderResourceView(_nativeTexture.Get(), &srvd, &_nativeShaderResourceView);
		ThrowIfBad(hr);
		ThrowIfBad(hr);
	}
}

auto Image::ShaderView() -> ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto Image::ShaderView() const -> const ID3D11ShaderResourceView&
{
	return *_nativeShaderResourceView.Get();
}

auto Image::Create(const ImageSpec& spec) -> std::shared_ptr<Image>
{
	return std::make_shared<Image>(spec);
}
}

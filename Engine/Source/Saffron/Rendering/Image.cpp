#include "SaffronPCH.h"

#include "Saffron/Rendering/Image.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/ErrorHandling/SaffronException.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Image::Image(const ImageSpec& spec, const uint* initialData) :
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

	if (bindFlags == 0)
	{
		throw SaffronException("Image must have a usage specified");
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
	if (initialData != nullptr)
	{
		sd.pSysMem = initialData;
	}

	auto hr = device.CreateTexture2D(&td, initialData ? &sd : nullptr, &_nativeTexture);
	ThrowIfBad(hr);

	if (depthStencil)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsd = {};
		dsd.Format = Utils::ToDxgiDepthStencilFormat(_spec.Format);
		dsd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsd.Texture2D.MipSlice = 0;

		ComPtr<ID3D11DepthStencilView> depthStencilView;
		hr = device.CreateDepthStencilView(_nativeTexture.Get(), &dsd, &depthStencilView);
		ThrowIfBad(hr);

		_nativeRenderView = depthStencilView;
		ThrowIfBad(hr);
	}
	else if (renderTarget)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rd = {};
		rd.Format = Utils::ToDxgiRenderTargetFormat(_spec.Format);
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
		srvd.Format = Utils::ToDxgiShaderResourceFormat(_spec.Format);
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = td.MipLevels;
		srvd.Texture2D.MostDetailedMip = 0;

		hr = device.CreateShaderResourceView(_nativeTexture.Get(), &srvd, &_nativeShaderResourceView);
		ThrowIfBad(hr);
	}
}

auto Image::ShaderView() -> ID3D11ShaderResourceView&
{
	Debug::Assert(_nativeShaderResourceView != nullptr, "No shader view for Image");
	return *_nativeShaderResourceView.Get();
}

auto Image::ShaderView() const -> const ID3D11ShaderResourceView&
{
	return const_cast<Image&>(*this).ShaderView();
}

auto Image::Width() const -> uint
{
	return _spec.Height;
}

auto Image::Height() const -> uint
{
	return _spec.Width;
}

auto Image::Format() const -> ImageFormat
{
	return _spec.Format;
}

auto Image::Create(const ImageSpec& spec) -> std::shared_ptr<Image>
{
	return std::make_shared<Image>(spec);
}

auto Image::CreateFromBackBuffer() -> std::shared_ptr<Image>
{
	auto result = std::unique_ptr<Image>(new Image);

	auto& device = Renderer::Device();
	auto& swapChain = Renderer::SwapChain();

	auto hr = swapChain.GetBuffer(0, __uuidof(ID3D11Texture2D), &result->_nativeTexture);
	ThrowIfBad(hr);

	ComPtr<ID3D11RenderTargetView> renderTargetView;
	hr = device.CreateRenderTargetView(result->_nativeTexture.Get(), nullptr, &renderTargetView);
	ThrowIfBad(hr);
	result->_nativeRenderView = renderTargetView;

	D3D11_TEXTURE2D_DESC td;
	result->_nativeTexture->GetDesc(&td);
	auto& spec = result->_spec;
	spec.Width = td.Width;
	spec.Height = td.Height;
	spec.Format = Utils::ToSaffronFormat(td.Format);
	spec.Usage = ImageUsage_RenderTarget;

	return result;
}
}

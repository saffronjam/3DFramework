#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Sampler.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Sampler::Sampler(const SamplerSpec& spec, uint slot) :
	_spec(spec),
	_slot(slot)
{
	SetInitializer(
		[this]
		{
			const auto inst = ShareThisAs<Sampler>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					const auto texAddressMode = Utils::ToD3D11TextureAddressMode(inst->_spec.Wrap);
					const auto filter = Utils::ToD3D11Filter(inst->_spec.Filter);

					D3D11_SAMPLER_DESC sd = {};
					sd.Filter = filter;
					sd.AddressU = texAddressMode;
					sd.AddressV = texAddressMode;
					sd.AddressW = texAddressMode;

					std::memcpy(sd.BorderColor, inst->_spec.BorderColor, sizeof(Color));

					const auto hr = package.Device.CreateSamplerState(&sd, &inst->_nativeSamplerState);
					ThrowIfBad(hr);
				}
			);
		}
	);
}

void Sampler::Bind() const
{
	const auto inst = ShareThisAs<const Sampler>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.CSSetSamplers(inst->_slot, 1, inst->_nativeSamplerState.GetAddressOf());
			package.Context.PSSetSamplers(inst->_slot, 1, inst->_nativeSamplerState.GetAddressOf());
		}
	);
}

void Sampler::Unbind() const
{
	const auto inst = ShareThisAs<const Sampler>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			constexpr ID3D11SamplerState* sampler = nullptr;
			package.Context.PSSetSamplers(inst->_slot, 1, &sampler);
		}
	);
}

auto Sampler::Create(const SamplerSpec& spec, uint slot) -> std::shared_ptr<Sampler>
{
	return BindableStore::Add<Sampler>(spec, slot);
}

auto Utils::ToD3D11TextureAddressMode(SamplerWrap edge) -> D3D11_TEXTURE_ADDRESS_MODE
{
	switch (edge)
	{
	case SamplerWrap::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
	case SamplerWrap::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
	case SamplerWrap::Border: return D3D11_TEXTURE_ADDRESS_BORDER;
	}
	throw SaffronException("Invalid sampler edge. Could not convert to D3D11_TEXTURE_ADDRESS_MODE.");
}

auto Utils::ToD3D11Filter(SamplerFilter filter) -> D3D11_FILTER
{
	switch (filter)
	{
	case SamplerFilter::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
	case SamplerFilter::Bilinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case SamplerFilter::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	throw SaffronException("Invalid sampler filter. Could not convert to D3D11_FILTER.");
}
}

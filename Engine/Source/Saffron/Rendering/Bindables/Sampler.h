#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
enum class SamplerWrap
{
	Mirror,
	Wrap,
	Border
};

enum class SamplerFilter
{
	Anisotropic,
	Bilinear,
	Point
};

struct SamplerSpec
{
	SamplerWrap Wrap = SamplerWrap::Mirror;
	SamplerFilter Filter = SamplerFilter::Bilinear;
	Color BorderColor = {0.0f, 0.0f, 0.0f, 0.0f};
};

class Sampler : public Bindable
{
public:
	explicit Sampler(const SamplerSpec& spec, uint slot);

	void Bind() const override;

	static auto Create(const SamplerSpec& spec = SamplerSpec{}, uint slot = 0) -> std::shared_ptr<Sampler>;

private:
	ComPtr<ID3D11SamplerState> _nativeSamplerState;

	SamplerSpec _spec;
	uint _slot;
};

namespace Utils
{
static auto ToD3D11TextureAddressMode(SamplerWrap edge) -> D3D11_TEXTURE_ADDRESS_MODE;
static auto ToD3D11Filter(SamplerFilter filter) -> D3D11_FILTER;
}
}

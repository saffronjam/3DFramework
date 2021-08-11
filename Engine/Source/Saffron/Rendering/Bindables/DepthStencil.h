#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class DepthStencil : public Bindable
{
public:
	DepthStencil();

	void Bind() const override;

	void SetEnabled(bool enabled);

	static auto Create() -> std::shared_ptr<DepthStencil>;

private:
	void Recreate();

private:
	ComPtr<ID3D11DepthStencilState> _nativeDepthStencilState;

	D3D11_DEPTH_STENCIL_DESC _descriptor{};
	mutable bool _dirty = false;
};
}

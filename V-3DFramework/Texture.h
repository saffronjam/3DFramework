#pragma once

#include "Bindable.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const Surface& surface);
	void BindTo(Graphics& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};


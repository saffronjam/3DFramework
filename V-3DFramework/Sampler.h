﻿#pragma once

#include "Bindable.h"

class Sampler : public Bindable
{
public:
	Sampler(Graphics& gfx);
	void BindTo(Graphics& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};

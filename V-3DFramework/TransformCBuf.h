#pragma once

#pragma once

#include "VeMath.h"
#include "ConstantBuffer.h"
#include "Drawable.h"

class TransformCBuf : public Bindable
{
public:
	TransformCBuf( Graphics &gfx, const Drawable &parent );
	void BindTo( Graphics &gfx ) noexcept override;

private:
	VSConstantBuffer<DirectX::XMMATRIX> m_vCBuf;
	const Drawable &m_parent;
};

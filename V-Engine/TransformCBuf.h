#pragma once

#pragma once

#include "VeMath.h"
#include "ConstantBuffer.h"
#include "Drawable.h"

namespace ve
{
class TransformCBuf : public Bindable
{
public:
	TransformCBuf( Graphics &gfx, const Drawable &parent );
	void Bind( Graphics &gfx ) noexcept override;

private:
	VSConstantBuffer<DirectX::XMMATRIX> m_vCBuf;
	const Drawable &m_parent;
};
}


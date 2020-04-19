#pragma once

#include <DirectXMath.h>

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
	VertexConstantBuffer<DirectX::XMMATRIX> m_vCBuf;
	const Drawable &m_parent;
};
}


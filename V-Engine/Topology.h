#pragma once

#include "Bindable.h"

namespace ve
{
class Topology : public Bindable
{
public:
	Topology( Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY type );
	void Bind( Graphics &gfx ) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY m_type;
};
}


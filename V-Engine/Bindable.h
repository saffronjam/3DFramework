#pragma once

#include "Graphics.h"

namespace ve
{
class Bindable
{
public:
	virtual void Bind( Graphics &gfx ) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11DeviceContext *GetContext( Graphics &gfx ) noexcept;
	static ID3D11Device *GetDevice( Graphics &gfx ) noexcept;
	static DXGI_InfoManager &GetInfoManager( Graphics &gfx ) noexcept( !IS_DEBUG );
};
}


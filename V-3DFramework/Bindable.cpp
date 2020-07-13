#include "Bindable.h"

ID3D11DeviceContext *Bindable::GetContext( Graphics &gfx ) noexcept
{
	return gfx.m_pContext.Get();
}

ID3D11Device *Bindable::GetDevice( Graphics &gfx ) noexcept
{
	return gfx.m_pDevice.Get();
}

DXGI_InfoManager &Bindable::GetInfoManager( Graphics &gfx ) noexcept( !IS_DEBUG )
{
#ifndef NDEBUG
	return gfx.m_infoManager;
#else
	throw std::logic_error( "Did you try to access the infoManager release config?" );
#endif
}

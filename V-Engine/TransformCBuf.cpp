#include "TransformCBuf.h"

ve::TransformCBuf::TransformCBuf( Graphics &gfx, const Drawable &parent )
	:
	m_vCBuf( gfx ),
	m_parent( parent )
{
}

void ve::TransformCBuf::Bind( Graphics &gfx ) noexcept
{
	m_vCBuf.Update(
		gfx,
		DirectX::XMMatrixTranspose(
			m_parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	m_vCBuf.Bind( gfx );
}

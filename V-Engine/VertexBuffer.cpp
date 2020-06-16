#include "VertexBuffer.h"

void VertexBuffer::Bind( Graphics &gfx ) noexcept
{
	const UINT offset = 0u;
	GetContext( gfx )->IASetVertexBuffers( 0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset );
}

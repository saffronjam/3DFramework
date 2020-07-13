#include "Topology.h"

Topology::Topology( Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY type )
	:
	m_type( type )
{
}

void Topology::BindTo( Graphics &gfx ) noexcept
{
	GetContext( gfx )->IASetPrimitiveTopology( m_type );
}

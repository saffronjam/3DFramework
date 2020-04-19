#include "Topology.h"

ve::Topology::Topology( Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY type )
	:
	m_type( type )
{
}

void ve::Topology::Bind( Graphics &gfx ) noexcept
{
	GetContext( gfx )->IASetPrimitiveTopology( m_type );
}

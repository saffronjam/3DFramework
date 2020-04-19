#pragma once

#include "Bindable.h"

namespace ve
{
class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer( Graphics &gfx, const std::vector<V> &vertices )
		:
		m_stride( sizeof( V ) )
	{
		INFOMAN( gfx );

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)( sizeof( V ) * vertices.size() );
		bd.StructureByteStride = m_stride;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();

		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &bd, &sd, &m_pVertexBuffer ) );
	}

	void Bind( Graphics &gfx ) noexcept override;
protected:
	UINT m_stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
};
}
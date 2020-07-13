#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template <typename C>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer( Graphics &gfx, const C &consts )
	{
		INFOMAN( gfx );

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof( consts );
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd, &csd, &m_pConstantBuffer ) );
	}
	ConstantBuffer( Graphics &gfx )
	{
		INFOMAN( gfx );

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof( C );
		cbd.StructureByteStride = 0u;
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd, nullptr, &m_pConstantBuffer ) );
	}


	void Update( Graphics &gfx, const C &consts )
	{
		INFOMAN( gfx );

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO( GetContext( gfx )->Map(
			m_pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		) );
		memcpy( msr.pData, &consts, sizeof( consts ) );
		GetContext( gfx )->Unmap( m_pConstantBuffer.Get(), 0u );
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
};

template <typename C>
class VSConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::m_pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void BindTo( Graphics &gfx ) noexcept override
	{
		GetContext( gfx )->VSSetConstantBuffers( 0u, 1u, m_pConstantBuffer.GetAddressOf() );
	}
};

template <typename C>
class PSConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::m_pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void BindTo( Graphics &gfx ) noexcept override
	{
		GetContext( gfx )->PSSetConstantBuffers( 0u, 1u, m_pConstantBuffer.GetAddressOf() );
	}
};
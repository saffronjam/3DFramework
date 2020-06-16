#pragma once

#include "VeMath.h"
#include "IndexedTriangleList.h"

class Cone
{
public:
	template<class V>
	static IndexedTriangleList<V> Make( unsigned int longDiv = 24u )
	{
		assert( "Number of longitude divisions must be at least 3" && longDiv >= 3 );

		const auto base = DirectX::XMVectorSet( 1.0f, 0.0f, -1.0f, 0.0f );
		const float longitudeAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;
		for ( int iLong = 0; iLong < longDiv; iLong++ )
		{
			vertices.emplace_back();
			auto vector3 = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ( longitudeAngle * iLong )
			);
			DirectX::XMStoreFloat3( &vertices.back().pos, vector3 );
		}
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto iCenter = (unsigned short)( vertices.size() - 1 );

		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iTip = (unsigned short)( vertices.size() - 1 );

		std::vector<unsigned short> indices;
		for ( unsigned short iLong = 0; iLong < longDiv; iLong++ )
		{
			indices.push_back( iCenter );
			indices.push_back( ( iLong + 1 ) % longDiv );
			indices.push_back( iLong );
		}

		for ( unsigned short iLong = 0; iLong < longDiv; iLong++ )
		{
			indices.push_back( iLong );
			indices.push_back( ( iLong + 1 ) % longDiv );
			indices.push_back( iTip );
		}

		return { std::move( vertices ),std::move( indices ) };
	}
};

#pragma once

#include <vector>

#include "VeMath.h"

template <typename T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList( std::vector<T> vertices_in, std::vector<unsigned short> indices_in )
		:
		vertices( std::move( vertices_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( "Number of vertices must be at least two" && vertices.size() > 2 );
		assert( "Indices must be divisible by 3" && indices.size() % 3 == 0 );
	}

	void Transform( DirectX::FXMMATRIX matrix )
	{
		for ( auto &vertex : vertices )
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3( &vertex.pos );
			DirectX::XMStoreFloat3(
				&vertex.pos,
				DirectX::XMVector3Transform( pos, matrix )
			);
		}
	}

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};

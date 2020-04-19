#pragma once

#include "VeMath.h"
#include "IndexedTriangleList.h"

namespace ve
{
class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		constexpr float side = 0.5f;

		std::vector<DirectX::XMFLOAT3> dx_vertices;
		dx_vertices.emplace_back( -side, -side, -side ); // 0
		dx_vertices.emplace_back( side, -side, -side ); // 1
		dx_vertices.emplace_back( -side, side, -side ); // 2
		dx_vertices.emplace_back( side, side, -side ); // 3
		dx_vertices.emplace_back( -side, -side, side ); // 4
		dx_vertices.emplace_back( side, -side, side ); // 5
		dx_vertices.emplace_back( -side, side, side ); // 6
		dx_vertices.emplace_back( side, side, side ); // 7

		std::vector<V> vertices( dx_vertices.size() );
		for ( size_t i = 0; i < dx_vertices.size(); i++ )
		{
			vertices[i].pos = dx_vertices[i];
		}
		return {
			std::move( vertices ),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
};
}


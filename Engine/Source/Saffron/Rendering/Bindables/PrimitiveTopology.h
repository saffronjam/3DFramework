#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
enum class PrimitiveTopologyType
{
	TriangleList
};

class PrimitiveTopology : public Bindable
{
public:
	PrimitiveTopology(PrimitiveTopologyType topology);

	void Bind() override;

private:
	D3D11_PRIMITIVE_TOPOLOGY _nativeTopology;
};

namespace Utils
{
inline auto ToD3D11PrimitiveTopology(PrimitiveTopologyType topology)
{
	switch (topology)
	{
	case PrimitiveTopologyType::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	default: break;
	}
	Debug::Break("Topology not supported");
	return static_cast<D3D11_PRIMITIVE_TOPOLOGY>(0);
}
}
}

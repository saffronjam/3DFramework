#pragma once

#include "Saffron/Core/Macros.h"
#include "Saffron/Core/TypeDefs.h"

namespace Se
{
struct RenderState
{
	RenderState() = default;
	RenderState(long state);

	auto operator==(const RenderState& other) const -> bool
	{
		return State == other.State;
	}

	auto operator!=(const RenderState& other) const -> bool
	{
		return !(State == other.State);
	}

	auto operator&(const RenderState& other) const -> bool
	{
		return State & other.State;
	}

	auto operator|(const RenderState& other) const -> bool
	{
		return State | other.State;
	}

	auto operator&(ulong other) const -> bool
	{
		return State & other;
	}

	auto operator|(ulong other) const -> bool
	{
		return State | other;
	}

	operator ulong() const
	{
		return State;
	}

	enum Enum : unsigned long long
	{
		// Depth test		
		DepthTest_Less = BIT(1),
		DepthTest_LessEqual = BIT(2),
		DepthTest_Equal = BIT(3),
		DepthTest_GreterEqual = BIT(4),
		DepthTest_Greter = BIT(5),
		DepthTest_NotEqual = BIT(6),
		DepthTest_Never = BIT(7),
		DepthTest_Always = BIT(8),

		DepthWriteMask_All = BIT(9),
		DepthWriteMask_Zero = BIT(10),

		// Rasterizer
		Rasterizer_CullFront = BIT(14),
		Rasterizer_CullBack = BIT(15),
		Rasterizer_CullNever = BIT(16),
		Rasterizer_Fill = BIT(17),
		Rasterizer_Wireframe = BIT(18),

		// Primitive topology
		Topology_TriangleList = BIT(22),
		Topology_LineList = BIT(23)
	};

	static const RenderState Default;

	ulong State = 0;
};
}

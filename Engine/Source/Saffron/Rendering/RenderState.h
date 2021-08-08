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

		// Rasterizer
		Rasterizer_CullFront = BIT(10),
		Rasterizer_CullBack = BIT(11),
		Rasterizer_CullNever = BIT(12),
		Rasterizer_Fill = BIT(13),
		Rasterizer_Wireframe = BIT(14),

		// Sampler
		Sampler_Anisotropic = BIT(15),
		Sampler_Bilinear = BIT(16),
		Sampler_Point = BIT(17)
	};

	static const RenderState Default;

	ulong State = 0;
};
}

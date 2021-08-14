#pragma once

#include "Saffron/Rendering/VertexLayout.h"

namespace Se
{
struct PosColVertex
{
	Vector3 Position;
	Color Color;

	static auto Layout() -> VertexLayout
	{
		return {{"Position", ElementType::Float3}, {"Color", ElementType::Float4}};
	}
};

struct PosColTexVertex
{
	Vector3 Position;
	Color Color;
	Vector2 TexCoords;

	static auto Layout() -> VertexLayout
	{
		return {{"Position", ElementType::Float3}, {"Color", ElementType::Float4}, {"TexCoord", ElementType::Float2}};
	}
};
}

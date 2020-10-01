#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
struct EditorViewport
{
	static glm::vec2 TopLeft;
	static glm::vec2 BottomRight;
	static bool Focused;
	static bool Hovered;

	static bool InViewport(glm::vec2 positionNDC);
	static glm::vec2 GetMousePosition();
};

}
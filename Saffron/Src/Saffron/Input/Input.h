#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Input/KeyCodes.h"
#include "Saffron/Input/MouseCodes.h"

namespace Se {

class Input
{
public:
	static bool IsKeyPressed(KeyCode keycode);

	static bool IsMouseButtonPressed(ButtonCode button);
	static float GetMouseX();
	static float GetMouseY();
	static glm::vec2 GetMousePosition();
};
}
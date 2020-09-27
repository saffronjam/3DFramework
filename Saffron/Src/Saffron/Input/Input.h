#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Core/Events/MouseEvent.h"
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
	static glm::vec2 GetMouseSwipe();

	static void OnUpdate();
	static void OnEvent(const Event &event);

private:
	static bool OnMouseMove(const MouseMoveEvent &event);

private:
	static glm::vec2 m_MousePosition;
	static glm::vec2 m_LastMousePosition;

};
}
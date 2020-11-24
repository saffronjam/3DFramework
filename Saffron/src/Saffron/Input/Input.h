#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Core/Events/MouseEvent.h"
#include "Saffron/Input/KeyCodes.h"
#include "Saffron/Input/MouseButtonCodes.h"

namespace Se {

class Input
{
public:
	static bool IsKeyPressed(KeyCode keycode);

	static bool IsMouseButtonPressed(MouseButtonCode button);
	static float GetMouseX();
	static float GetMouseY();
	static Vector2f GetMousePosition();
	static Vector2f GetMousePositionNDC();
	static Vector2f GetMouseSwipe();

	static void OnUpdate();
	static void OnEvent(const Event &event);

private:
	static bool OnMouseMove(const MouseMoveEvent &event);

private:
	static Vector2f m_MousePosition;
	static Vector2f m_LastMousePosition;

};
}
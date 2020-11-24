#include "SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Platform/Windows/WindowsWindow.h"


namespace Se
{

Vector2f Input::m_MousePosition = { 0.0f, 0.0f };
Vector2f Input::m_LastMousePosition = { 0.0f, 0.0f };

bool Input::IsKeyPressed(KeyCode keycode)
{
	auto &window = dynamic_cast<WindowsWindow &>(Application::Get().GetWindow());
	const auto state = glfwGetKey(static_cast<GLFWwindow *>(window.GetNativeWindow()), static_cast<Int32>(keycode));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(MouseButtonCode button)
{
	auto &window = dynamic_cast<WindowsWindow &>(Application::Get().GetWindow());
	const auto state = glfwGetMouseButton(static_cast<GLFWwindow *>(window.GetNativeWindow()), static_cast<int>(button));
	return state == GLFW_PRESS;
}

float Input::GetMouseX()
{
	return m_MousePosition.x;
}

float Input::GetMouseY()
{
	return m_MousePosition.y;
}

Vector2f Input::GetMousePosition()
{
	return m_MousePosition;
}

Vector2f Input::GetMousePositionNDC()
{
	const ImVec2 mousePos = ImGui::GetMousePos();
	return { mousePos.x, mousePos.y };
}

Vector2f Input::GetMouseSwipe()
{
	Vector2f swipe = m_MousePosition - m_LastMousePosition;
	if ( glm::length(swipe) > 100.0f )
		swipe = { 0.0f, 0.0f };
	return swipe;
}

void Input::OnUpdate()
{
	m_LastMousePosition = m_MousePosition;
}

void Input::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<MouseMoveEvent>(OnMouseMove);
}

bool Input::OnMouseMove(const MouseMoveEvent &event)
{
	m_MousePosition = event.GetPosition();
	return false;
}
}

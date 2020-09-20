#include "SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Platform/Windows/WindowsWindow.h"


namespace Se {

bool Input::IsKeyPressed(KeyCode keycode)
{
	auto &window = dynamic_cast<WindowsWindow &>(Application::Get().GetWindow());
	const auto state = glfwGetKey(static_cast<GLFWwindow *>(window.GetNativeWindow()), static_cast<Int32>(keycode));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(ButtonCode button)
{
	auto &window = dynamic_cast<WindowsWindow &>(Application::Get().GetWindow());
	const auto state = glfwGetMouseButton(static_cast<GLFWwindow *>(window.GetNativeWindow()), static_cast<int>(button));
	return state == GLFW_PRESS;
}

float Input::GetMouseX()
{
	return static_cast<float>(GetMousePosition().x);
}

float Input::GetMouseY()
{
	return static_cast<float>(GetMousePosition().y);
}

glm::vec2 Input::GetMousePosition()
{
	auto &window = dynamic_cast<WindowsWindow &>(Application::Get().GetWindow());

	double x, y;
	glfwGetCursorPos(static_cast<GLFWwindow *>(window.GetNativeWindow()), &x, &y);
	return { static_cast<float>(x), static_cast<float>(y) };

}

}
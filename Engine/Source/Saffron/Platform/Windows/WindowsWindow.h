#pragma once

#include <Windows.h>

#include "Saffron/Common/Window.h"

namespace Se
{
class WindowsWindow : public Window
{
public:
	explicit WindowsWindow(const WindowSpec& spec);

	void OnUpdate() override;

	auto NativeHandle() const -> void* override;

private:
	static auto WndSetup(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam) -> LRESULT;
	static auto WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam) -> LRESULT;
	auto HandleWin32Message(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam) -> LRESULT;

private:
	HINSTANCE _hInstance = nullptr;
	HWND _hWnd = nullptr;
};

namespace Utils
{
KeyCode VKToKeyCode(unsigned char vkKey);
}
}

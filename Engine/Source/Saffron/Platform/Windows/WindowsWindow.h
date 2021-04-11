#pragma once

#include "Saffron/Common/Window.h"
#include "Saffron/Common/Events/WindowEvent.h"

namespace Se
{
class WindowsWindow : public Window
{
public:
	explicit WindowsWindow(const Properties& props);
	virtual ~WindowsWindow();

	void OnUpdate() override;
	void OnEvent(const Event& event) override;

	void Close() override;
	void Focus() override;

	void* GetNativeWindow() const override;

	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	void Minimize() override;
	void Maximize() override;
	bool IsMinimized() const override;
	bool IsMaximized() const override;

private:
	void EnableCursor() override;
	void DisableCursor() override;

	void ConfineCursor() override;
	void FreeCursor() override;

	void HideCursor() override;
	void ShowCursor() override;

private:
	bool OnResize(const WindowResizedEvent& event);
	bool OnMove(const WindowMovedEvent& event);
	bool OnGainFocus(const WindowGainedFocusEvent& event);
	bool OnLostFocus(const WindowLostFocusEvent& event);
	bool OnClose(const WindowClosedEvent& event);
	bool OnNewTitle(const WindowNewTitleEvent& event);
	bool OnNewIcon(const WindowNewIconEvent& event);
	bool OnNewAntiAliasing(const WindowNewAntiAliasingEvent& event);

	void Restore();

	static LRESULT WndSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleWin32Message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	KeyCode VKToKeyCode(unsigned char vkkey);
	Vector2 Win32PointToVec(LPARAM lParam);

private:
	HWND _hWnd;
	HINSTANCE _hInstance;

	bool _vsync, _minimized, _maximized;

	ArrayList<unsigned char> _rawInputBuffer;
};
}

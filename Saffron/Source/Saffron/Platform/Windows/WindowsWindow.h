#pragma once

#include "Saffron/Core/Window.h"
#include "Saffron/Core/Events/WindowEvent.h"

struct GLFWwindow;
struct GLFWcursor;

namespace Se
{
class WindowsWindow : public Window
{
public:
	explicit WindowsWindow(const Properties &props);
	virtual ~WindowsWindow();

	void OnUpdate() override;
	void OnEvent(const Event &event) override;

	void Close() override;
	void Focus() override;

	void *GetNativeWindow() const override;

	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	void Minimize() override;
	void Maximize() override;
	bool IsMinimized() const override;
	bool IsMaximized() const override;

private:
	bool OnResize(const WindowResizeEvent &event);
	bool OnMove(const WindowMoveEvent &event);
	bool OnGainFocus(const WindowGainFocusEvent &event);
	bool OnLostFocus(const WindowLostFocusEvent &event);
	bool OnClose(const WindowCloseEvent &event);
	bool OnNewTitle(const WindowNewTitleEvent &event);
	bool OnNewIcon(const WindowNewIconEvent &event);
	bool OnNewAntiAliasing(const WindowNewAntiAliasingEvent &event);

	void SetupGLFWCallbacks();

	void Restore();

private:
	GLFWwindow *m_NativeWindow;
	GLFWcursor *m_ImGuiMouseCursors[9] = { nullptr };

	bool m_VSync, m_Minimized, m_Maximized;
};
}
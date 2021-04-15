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
	bool OnResize(const WindowResizedEvent& event);
	bool OnMove(const WindowMovedEvent& event);
	bool OnGainFocus(const WindowGainedFocusEvent& event);
	bool OnLostFocus(const WindowLostFocusEvent& event);
	bool OnClose(const WindowClosedEvent& event);
	bool OnNewTitle(const WindowNewTitleEvent& event);
	bool OnNewIcon(const WindowNewIconEvent& event);

	void SetupGLFWCallbacks();

	void Restore();

private:
	GLFWwindow* _nativeWindow;
	GLFWcursor* _imGuiMouseCursors[9] = {nullptr};

	bool _vsync, _minimized, _maximized;
};
}
